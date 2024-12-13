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
	if (!ensure(InType)) return 0;
	int32& Count = ReserveAmmoTypeToCount.FindOrAdd(InType);
	if (InAmmoCount > 0)
	{
		Count = FMath::Min(Count + InAmmoCount, ReserveMaxAmmo);
		ReserveAmmoChangedEvent.Broadcast(Count, ReserveMaxAmmo);
	}
	return Count;
}

int32 UMETAmmoManager::TryConsumeReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount)
{
	if (!ensure(InType)) return 0;
	int32& Count = ReserveAmmoTypeToCount.FindOrAdd(InType);
	
	const int32 OriginalCount = Count;
	const int32 ConsumeCount = InConsumeCount > 0 ? InConsumeCount : 0;
	Count = FMath::Max(0, OriginalCount - ConsumeCount);

	ReserveAmmoChangedEvent.Broadcast(Count, ReserveMaxAmmo);

	return FMath::Min(OriginalCount, ConsumeCount);
}

void UMETAmmoManager::WeaponAmmoChanged(const int32 InAmmoCount, const int32 InMaxAmmo) const
{
	WeaponAmmoChangedEvent.Broadcast(InAmmoCount, InMaxAmmo);
}

void UMETAmmoManager::WeaponAmmoTypeChanged(UMETAmmoDataAsset* const InType)
{
	const int32 AmmoCount = GetReserveAmmoCount(InType);
	ReserveAmmoChangedEvent.Broadcast(AmmoCount, ReserveMaxAmmo);
}
