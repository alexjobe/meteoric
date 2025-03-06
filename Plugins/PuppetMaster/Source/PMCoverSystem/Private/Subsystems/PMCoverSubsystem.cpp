// Copyright Alex Jobe


#include "Subsystems/PMCoverSubsystem.h"

#include "Components/PMCoverSpot.h"
#include "Logging/PMCoverSystemLog.h"

void UPMCoverSubsystem::AddActiveCoverSpot(const UPMCoverSpot* InSpot)
{
	if (InSpot == nullptr) return;
	bool bIsAlreadyActive = false;
	ActiveCoverSpots.Emplace(InSpot, &bIsAlreadyActive);
	if (!ensure(!bIsAlreadyActive))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSubsystem::AddActiveCoverSpot -- Cover spot already active! %s"),
			*InSpot->GetName());
	}
}

void UPMCoverSubsystem::RemoveActiveCoverSpot(const UPMCoverSpot* const InSpot)
{
	if (InSpot == nullptr) return;
	const int32 NumRemoved = ActiveCoverSpots.Remove(InSpot);
	if (!ensure(NumRemoved > 0))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSubsystem::RemoveActiveCoverSpot -- Unable to remove cover spot! %s"),
			*InSpot->GetName());
	}
}

UPMCoverSubsystem* UPMCoverSubsystem::GetSubsystem(const UObject* WorldContextObject)
{
	const UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;
	const UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	return GameInstance ? GameInstance->GetSubsystem<UPMCoverSubsystem>() : nullptr;
}
