// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PMCoverSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PMCOVERSYSTEM_API UPMCoverSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void AddOccupiedCoverSpot(const class UPMCoverSpot* InSpot);
	void RemoveOccupiedCoverSpot(const UPMCoverSpot* InSpot);

	void AddClaimedCoverSpot(const UPMCoverSpot* InSpot);
	void RemoveClaimedCoverSpot(const UPMCoverSpot* InSpot);

	const TSet<const UPMCoverSpot*>& GetOccupiedCoverSpots() const { return OccupiedCoverSpots; }
	const TSet<const UPMCoverSpot*>& GetClaimedCoverSpots() const { return ClaimedCoverSpots; }
	
	static UPMCoverSubsystem* GetSubsystem(const UObject* WorldContextObject);

private:
	UPROPERTY(Transient)
	TSet<const UPMCoverSpot*> OccupiedCoverSpots;

	UPROPERTY(Transient)
	TSet<const UPMCoverSpot*> ClaimedCoverSpots;

	static void AddCoverSpot(const UPMCoverSpot* InSpot, TSet<const UPMCoverSpot*>& InSet);
	static void RemoveCoverSpot(const UPMCoverSpot* InSpot, TSet<const UPMCoverSpot*>& InSet);
};
