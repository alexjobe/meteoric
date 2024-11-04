// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "METWeapon.h"
#include "GameFramework/Character.h"


UMETWeaponManager::UMETWeaponManager()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETWeaponManager::InitializeComponent()
{
	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon)
{
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;

	CurrentWeapon = InWeapon;
	CurrentWeapon->OnEquipped(OwningCharacter);
	CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	WeaponEquippedEvent.Broadcast(CurrentWeapon);
}