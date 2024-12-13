// Copyright Alex Jobe


#include "METWeaponAmmoComponent.h"

#include "METAmmoManager.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponAmmoComponent::UMETWeaponAmmoComponent()
	: MaxAmmo(20)
	, AmmoCount(20)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETWeaponAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	AmmoCount = MaxAmmo;
}

int32 UMETWeaponAmmoComponent::Reload()
{
	if (!ensure(AmmoManager)) return AmmoCount;

	const int32 ReloadAmount = AmmoManager->TryConsumeReserveAmmo(CurrentAmmoType, FMath::Max(0, MaxAmmo - AmmoCount));
	AmmoCount += ReloadAmount;
	AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
	return AmmoCount;
}

bool UMETWeaponAmmoComponent::TryConsumeAmmo(const int32 InConsumeCount)
{
	if (!ensure(AmmoManager)) return false;
	if (InConsumeCount < 0) return false;
	
	bool bConsumed = false;
	if (const int32 NewCount = AmmoCount - InConsumeCount; NewCount >= 0)
	{
		AmmoCount = NewCount;
		bConsumed = true;
		AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
	}
	return bConsumed;
}

void UMETWeaponAmmoComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	if (!ensure(InOwningCharacter)) return;

	OwningCharacter = InOwningCharacter;
	AmmoManager = OwningCharacter->FindComponentByClass<UMETAmmoManager>();
	if (ensure(AmmoManager))
	{
		AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
		AmmoManager->WeaponAmmoTypeChanged(CurrentAmmoType);
	}
}

void UMETWeaponAmmoComponent::OnWeaponUnequipped()
{
	if (AmmoManager)
	{
		AmmoManager->WeaponAmmoChanged(0, 0);
		AmmoManager->WeaponAmmoTypeChanged(nullptr);
	}
	OwningCharacter = nullptr;
	AmmoManager = nullptr;
}

void UMETWeaponAmmoComponent::OnRep_OwningCharacter(ACharacter* InOldOwner)
{
	if (OwningCharacter)
	{
		AmmoManager = OwningCharacter->FindComponentByClass<UMETAmmoManager>();
	}
	else
	{
		AmmoManager = nullptr;
	}
}

void UMETWeaponAmmoComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMETWeaponAmmoComponent, CurrentAmmoType);
	DOREPLIFETIME(UMETWeaponAmmoComponent, AmmoCount);
	DOREPLIFETIME(UMETWeaponAmmoComponent, OwningCharacter);
}
