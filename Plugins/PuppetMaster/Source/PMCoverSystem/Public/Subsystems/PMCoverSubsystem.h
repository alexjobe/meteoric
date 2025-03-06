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
	void AddActiveCoverSpot(const class UPMCoverSpot* InSpot);
	void RemoveActiveCoverSpot(const UPMCoverSpot* InSpot);

	const TSet<const UPMCoverSpot*>& GetActiveCoverSpots() const { return ActiveCoverSpots; }
	
	static UPMCoverSubsystem* GetSubsystem(const UObject* WorldContextObject);

private:
	UPROPERTY(Transient)
	TSet<const UPMCoverSpot*> ActiveCoverSpots;
};
