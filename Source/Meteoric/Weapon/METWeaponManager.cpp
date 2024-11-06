// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "EnhancedInputComponent.h"
#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponManager::UMETWeaponManager()
	: MaxWeapons(2)
	, SelectedWeaponSlot(0)
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

void UMETWeaponManager::StartEquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	if(!OwningCharacter->HasAuthority()) return;

	UnequipWeapon(CurrentWeapon);
	CurrentWeapon = InWeapon;
	EquipWeapon(CurrentWeapon);

	Weapons[InSlot] = CurrentWeapon;
	SelectedWeaponSlot = InSlot;
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon) const
{
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	InWeapon->OnEquipped(OwningCharacter);
	InWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InWeapon->ParentAttachmentSocket);
	WeaponEquippedEvent.Broadcast(InWeapon);
	InWeapon->GetMesh()->SetVisibility(true, true);
}

void UMETWeaponManager::UnequipWeapon(AMETWeapon* const InWeapon)
{
	if(!InWeapon) return;
	InWeapon->OnUnequipped();
	InWeapon->GetMesh()->SetVisibility(false, true);
}

void UMETWeaponManager::OnRep_CurrentWeapon(AMETWeapon* const InOldWeapon) const
{
	if(InOldWeapon)
	{
		UnequipWeapon(InOldWeapon);
	}
	EquipWeapon(CurrentWeapon);
}

void UMETWeaponManager::CycleWeapon(bool bInNext)
{
	if(MaxWeapons <= 1) return;
	
	int NewSlot = SelectedWeaponSlot + (bInNext ? 1 : -1);
	if(NewSlot < 0) NewSlot = MaxWeapons - 1;
	if(NewSlot >= MaxWeapons) NewSlot = 0;

	SelectedWeaponSlot = NewSlot;
	
	if(Weapons[NewSlot] != nullptr)
	{
		StartEquipWeapon(Weapons[NewSlot], NewSlot);
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
		EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &UMETWeaponManager::CycleWeaponInput);
	}
}
