// Copyright Alex Jobe


#include "METAmmoManager.h"

UMETAmmoManager::UMETAmmoManager()
	: ReserveMaxAmmo(1000)
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UMETAmmoManager::GetReserveAmmoCount(UMETAmmoDataAsset* const InType)
{
	if (!InType) return 0;
	return ReserveAmmoTypeToCount.FindOrAdd(InType);
}

int32 UMETAmmoManager::AddReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InAmmoCount)
{
	if (!ensure(InType) || !GetOwner()->HasAuthority()) return 0;
	int32& AmmoCount = ReserveAmmoTypeToCount.FindOrAdd(InType);
	if (InAmmoCount > 0)
	{
		AmmoCount = FMath::Min(AmmoCount + InAmmoCount, ReserveMaxAmmo);
		ReserveAmmoChangedEvent.Broadcast(AmmoCount, ReserveMaxAmmo);
		Client_SetReserveAmmo(InType, AmmoCount, ReserveMaxAmmo);
	}
	return AmmoCount;
}

int32 UMETAmmoManager::TryConsumeReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount)
{
	if (!ensure(InType) || !GetOwner()->HasAuthority()) return 0;
	int32& AmmoCount = ReserveAmmoTypeToCount.FindOrAdd(InType);
	
	const int32 OriginalCount = AmmoCount;
	const int32 ConsumeCount = InConsumeCount > 0 ? InConsumeCount : 0;
	AmmoCount = FMath::Max(0, OriginalCount - ConsumeCount);

	ReserveAmmoChangedEvent.Broadcast(AmmoCount, ReserveMaxAmmo);
	Client_SetReserveAmmo(InType, AmmoCount, ReserveMaxAmmo);

	return FMath::Min(OriginalCount, ConsumeCount);
}

void UMETAmmoManager::WeaponAmmoChanged(const int32 InAmmoCount, const int32 InMaxAmmo) const
{
	WeaponAmmoChangedEvent.Broadcast(InAmmoCount, InMaxAmmo);
	if (GetOwner()->HasAuthority())
	{
		Client_WeaponAmmoChanged(InAmmoCount, InMaxAmmo);
	}
}

void UMETAmmoManager::WeaponAmmoTypeChanged(UMETAmmoDataAsset* const InType)
{
	const int32 AmmoCount = GetReserveAmmoCount(InType);
	ReserveAmmoChangedEvent.Broadcast(AmmoCount, ReserveMaxAmmo);
	if (GetOwner()->HasAuthority())
	{
		Client_WeaponAmmoTypeChanged(InType);
	}
}

void UMETAmmoManager::Client_WeaponAmmoTypeChanged_Implementation(UMETAmmoDataAsset* InType)
{
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		WeaponAmmoTypeChanged(InType);
	}
}

void UMETAmmoManager::Client_WeaponAmmoChanged_Implementation(const int32 InAmmoCount, const int32 InMaxAmmo) const
{
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		WeaponAmmoChanged(InAmmoCount, InMaxAmmo);
	}
}

void UMETAmmoManager::Client_SetReserveAmmo_Implementation(UMETAmmoDataAsset* InType, const int32 InAmmoCount, const int32 InMaxAmmo)
{
	if (!ensure(InType)) return;
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		int32& Count = ReserveAmmoTypeToCount.FindOrAdd(InType);
		Count = InAmmoCount;
		ReserveMaxAmmo = InMaxAmmo;
		ReserveAmmoChangedEvent.Broadcast(Count, ReserveMaxAmmo);
	}
}
