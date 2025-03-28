// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PMCoverSystemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PMCoverSystemUtils.generated.h"

/**
 * 
 */
UCLASS()
class PMCOVERSYSTEM_API UPMCoverSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FVector GetKeyLocation(const class UBlackboardComponent* BlackboardComp, const struct FBlackboardKeySelector& Key);
	static float GetKeyFloatValue(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key);
	static class UPMCoverSpot* FindCover(const FPMFindCoverParams& Params);

private:
	// Find all cover actors (IPMCoverInterface) in search radius
	static TArray<AActor*>FindCoverActors(const FPMFindCoverParams& Params);

	// Filter out cover actors that do not fall within ideal distance to target
	static TArray<AActor*> FilterCoverActors(TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params);

	// Sort cover actors by search mode
	static void SortCoverActors(TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params);

	// Once cover actors are sorted, iterate until we find one with a valid cover spot
	static UPMCoverSpot* GetBestCoverSpot(const TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params);
};
