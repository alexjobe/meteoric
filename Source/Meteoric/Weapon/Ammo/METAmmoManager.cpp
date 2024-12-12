// Copyright Alex Jobe


#include "METAmmoManager.h"

UMETAmmoManager::UMETAmmoManager()
	: MaxCount(1000)
{
	PrimaryComponentTick.bCanEverTick = false;
}

int32 UMETAmmoManager::GetAmmoCount(UMETAmmoDataAsset* const InType)
{
	if (!ensure(InType)) return 0;
	return AmmoTypeToCount.FindOrAdd(InType);
}

int32 UMETAmmoManager::AddAmmo(UMETAmmoDataAsset* const InType, const int32 InAmmoCount)
{
	if (!ensure(InType)) return 0;
	int32& Count = AmmoTypeToCount.FindOrAdd(InType);
	if (InAmmoCount > 0)
	{
		Count = FMath::Min(Count + InAmmoCount, MaxCount);
	}
	return Count;
}

int32 UMETAmmoManager::TryConsumeAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount)
{
	if (!ensure(InType)) return 0;
	int32& Count = AmmoTypeToCount.FindOrAdd(InType);
	
	const int32 OriginalCount = Count;
	const int32 ConsumeCount = InConsumeCount > 0 ? InConsumeCount : 0;
	Count = FMath::Max(0, OriginalCount - ConsumeCount);

	return FMath::Min(OriginalCount, ConsumeCount);
}
