// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "EnhancedInputComponent.h"
#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
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
	if(UMETInteractionComponent* InteractionComponent = OwningCharacter->FindComponentByClass<UMETInteractionComponent>())
	{
		InteractionComponent->OnInteractEvent().AddUniqueDynamic(this, &UMETWeaponManager::InteractionComponent_OnInteractEvent);
	}
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon) || InWeapon == CurrentWeapon) return;
	if(bIsChangingWeapons) return;

	if(OwningCharacter->HasAuthority())
	{
		bIsChangingWeapons = true;
		ChangingWeaponsEvent.Broadcast(bIsChangingWeapons);
		
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
		const UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
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
	CurrentWeapon->SetWeaponDropped(false);
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

int UMETWeaponManager::ChooseEquipSlot() const
{
	if(Weapons[SelectedWeaponSlot] != nullptr)
	{
		for(int Slot = 0; Slot < MaxWeapons; ++Slot)
		{
			if(Weapons[Slot] == nullptr) return Slot;
		}
	}

	return SelectedWeaponSlot;
}

void UMETWeaponManager::InteractionComponent_OnInteractEvent(AActor* InInteractable)
{
	AMETWeapon* NewWeapon = Cast<AMETWeapon>(InInteractable);
	if(!NewWeapon) return;

	const int EquipSlot = ChooseEquipSlot();
	if(Weapons[EquipSlot] == CurrentWeapon && CurrentWeapon != nullptr)
	{
		CurrentWeapon->Drop();
		Weapons[EquipSlot] = nullptr;
		CurrentWeapon = nullptr;
		PreviousWeapon = nullptr;
	}
	EquipWeapon(NewWeapon, EquipSlot);
}

void UMETWeaponManager::Server_CycleWeapon_Implementation(bool bInNext)
{
	CycleWeapon(bInNext);
}

void UMETWeaponManager::CycleWeaponInput(const FInputActionValue& Value)
{
	const bool bNext = Value.Get<float>() > 0.f;;

	if(GetOwner()->HasAuthority())
	{
		CycleWeapon(bNext);
	}
	else
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
