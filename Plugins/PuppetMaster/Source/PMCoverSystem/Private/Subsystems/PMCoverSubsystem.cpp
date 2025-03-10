// Copyright Alex Jobe


#include "Subsystems/PMCoverSubsystem.h"

#include "Components/PMCoverSpot.h"
#include "Logging/PMCoverSystemLog.h"

void UPMCoverSubsystem::AddOccupiedCoverSpot(const UPMCoverSpot* InSpot)
{
	AddCoverSpot(InSpot, OccupiedCoverSpots);
}

void UPMCoverSubsystem::RemoveOccupiedCoverSpot(const UPMCoverSpot* const InSpot)
{
	RemoveCoverSpot(InSpot, OccupiedCoverSpots);
}

void UPMCoverSubsystem::AddClaimedCoverSpot(const UPMCoverSpot* InSpot)
{
	AddCoverSpot(InSpot, ClaimedCoverSpots);
}

void UPMCoverSubsystem::RemoveClaimedCoverSpot(const UPMCoverSpot* InSpot)
{
	RemoveCoverSpot(InSpot, ClaimedCoverSpots);
}

UPMCoverSubsystem* UPMCoverSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UPMCoverSubsystem>() : nullptr;
}

void UPMCoverSubsystem::AddCoverSpot(const UPMCoverSpot* InSpot, TSet<const UPMCoverSpot*>& InSet)
{
	if (!ensure(InSpot)) return;
	bool bIsAlreadyActive = false;
	InSet.Emplace(InSpot, &bIsAlreadyActive);
	if (!ensure(!bIsAlreadyActive))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSubsystem::AddCoverSpot -- Cover spot already found! %s"),
			*InSpot->GetName());
	}
}

void UPMCoverSubsystem::RemoveCoverSpot(const UPMCoverSpot* InSpot, TSet<const UPMCoverSpot*>& InSet)
{
	if (!ensure(InSpot)) return;
	const int32 NumRemoved = InSet.Remove(InSpot);
	if (!ensure(NumRemoved > 0))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSubsystem::RemoveCoverSpot -- Unable to remove cover spot! %s"),
			*InSpot->GetName());
	}
}
