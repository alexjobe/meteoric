// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponManager::UMETWeaponManager()
	: MaxWeapons(4)
	, CurrentWeaponSlot(0)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Weapons.Init(nullptr, MaxWeapons);
}

void UMETWeaponManager::InitializeComponent()
{
	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;

	UnequipCurrentWeapon();

	CurrentWeapon = InWeapon;
	CurrentWeapon->OnEquipped(OwningCharacter);
	CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	WeaponEquippedEvent.Broadcast(CurrentWeapon);

	CurrentWeapon->GetMesh()->SetVisibility(true, true);

	Weapons[InSlot] = CurrentWeapon;
	CurrentWeaponSlot = InSlot;
}

void UMETWeaponManager::UnequipCurrentWeapon()
{
	if(!CurrentWeapon) return;
	CurrentWeapon->OnUnequipped();
	CurrentWeapon->GetMesh()->SetVisibility(false, true);
	CurrentWeapon = nullptr;
}

void UMETWeaponManager::CycleWeapon()
{
	int NewSlot = (CurrentWeaponSlot + 1) % MaxWeapons;
	while(NewSlot != CurrentWeaponSlot)
	{
		if(Weapons[NewSlot] != nullptr)
		{
			EquipWeapon(Weapons[NewSlot], NewSlot);
			return;
		}
		NewSlot = (NewSlot + 1) % MaxWeapons;
	}
}