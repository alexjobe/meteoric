// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "EnhancedInputComponent.h"
#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponManager::UMETWeaponManager()
	: MaxWeapons(2)
	, SelectedWeaponSlot(0)
	, bIsChangingWeapons(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Weapons.Init(nullptr, MaxWeapons);
	SetIsReplicatedByDefault(true);
}

void UMETWeaponManager::InitializeComponent()
{
	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	if(bIsChangingWeapons) return;

	bIsChangingWeapons = true;
	ChangingWeaponsEvent.Broadcast(bIsChangingWeapons);

	if(OwningCharacter->HasAuthority())
	{
		PreviousWeapon = CurrentWeapon;
		StartEquipWeapon(InWeapon);

		Weapons[InSlot] = CurrentWeapon;
		SelectedWeaponSlot = InSlot;
	}
}

void UMETWeaponManager::StartEquipWeapon(AMETWeapon* const InWeapon)
{
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	CurrentWeapon = InWeapon;
	OwningCharacter->PlayAnimMontage(CurrentWeapon->GetCharacterEquipWeaponMontage());

	if(UAnimMontage* EquipMontage = CurrentWeapon->GetCharacterEquipWeaponMontage())
	{
		UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
		if(FAnimMontageInstance* MontageInstance = AnimInstance ? AnimInstance->GetInstanceForMontage(EquipMontage) : nullptr)
		{
			MontageInstance->OnMontageBlendingOutStarted.BindLambda([this](UAnimMontage*, bool)
			{
				FinishEquipWeapon();
			});
		}
	}
}

void UMETWeaponManager::OnEquipWeaponNotify()
{
	if(!ensure(OwningCharacter) || !ensure(CurrentWeapon)) return;
	UnequipWeapon(PreviousWeapon);
	PreviousWeapon = nullptr;
	CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	CurrentWeapon->GetMesh()->SetVisibility(true, true);
	CurrentWeapon->OnEquipped(OwningCharacter);
	WeaponEquippedEvent.Broadcast(CurrentWeapon);
}

void UMETWeaponManager::FinishEquipWeapon()
{
	bIsChangingWeapons = false;
	ChangingWeaponsEvent.Broadcast(bIsChangingWeapons);
}

void UMETWeaponManager::UnequipWeapon(AMETWeapon* const InWeapon)
{
	if(!InWeapon) return;
	InWeapon->OnUnequipped();
	InWeapon->GetMesh()->SetVisibility(false, true);
}

void UMETWeaponManager::OnRep_CurrentWeapon(AMETWeapon* const InOldWeapon)
{
	if(InOldWeapon)
	{
		PreviousWeapon = InOldWeapon;
	}

	if(CurrentWeapon)
	{
		StartEquipWeapon(CurrentWeapon);
	}
}

void UMETWeaponManager::CycleWeapon(bool bInNext)
{
	if(MaxWeapons <= 1) return;
	if(bIsChangingWeapons) return;
	
	int NewSlot = SelectedWeaponSlot + (bInNext ? 1 : -1);
	if(NewSlot < 0) NewSlot = MaxWeapons - 1;
	if(NewSlot >= MaxWeapons) NewSlot = 0;

	SelectedWeaponSlot = NewSlot;
	
	if(Weapons[NewSlot] != nullptr)
	{
		EquipWeapon(Weapons[NewSlot], NewSlot);
	}
}

void UMETWeaponManager::Server_CycleWeapon_Implementation(bool bInNext)
{
	CycleWeapon(bInNext);
}

void UMETWeaponManager::CycleWeaponInput(const FInputActionValue& Value)
{
	const bool bNext = Value.Get<float>() > 0.f;;
	CycleWeapon(bNext);

	if(!GetOwner()->HasAuthority())
	{
		Server_CycleWeapon(bNext);
	}
}

void UMETWeaponManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMETWeaponManager, CurrentWeapon)
	DOREPLIFETIME(UMETWeaponManager, OwningCharacter)
	DOREPLIFETIME(UMETWeaponManager, Weapons)
}

void UMETWeaponManager::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &UMETWeaponManager::CycleWeaponInput);
	}
}
