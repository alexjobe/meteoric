// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BTTaskNode.h"
#include "PMBTTask_FindCover.generated.h"


UENUM(BlueprintType)
enum class EPMCoverSearchMode : uint8
{
	LeastDistanceToQuerier,
	GreatestDistanceToQuerier,
	LeastDistanceToTarget,
	GreatestDistanceToTarget
};

/**
 * 
 */
UCLASS(MinimalAPI)
class UPMBTTask_FindCover : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	PMCOVERSYSTEM_API UPMBTTask_FindCover();

	//~ Begin UBTTaskNode interface
	PMCOVERSYSTEM_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	PMCOVERSYSTEM_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTTaskNode interface

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector SearchCenterKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector MinDistanceToTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector MaxDistanceToTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector MoveToLocationKey;

	UPROPERTY(EditAnywhere, Category = "Search") 
	EPMCoverSearchMode SearchMode;

	UPROPERTY(EditAnywhere, Category = "Search") 
	float SearchRadius;

	// Test if querier can reach cover spot -- potentially expensive
	UPROPERTY(EditAnywhere, Category = "Search") 
	bool bTestCoverSpotNavigable;

	// Find all cover actors (IPMCoverInterface) in search radius
	static TArray<AActor*>FindCoverActors(const AActor* ContextActor, const FVector& SearchCenter, const float& SearchRadius);

	// Filter out cover actors that do not fall within ideal distance to target
	static TArray<AActor*> FilterCoverActors(TArray<AActor*>& CoverActors, const FVector& Target, float MinDistance, float MaxDistance);

	// Sort cover actors by search mode
	static void SortCoverActors(TArray<AActor*>& CoverActors, const FVector& TargetLocation, const FVector& QuerierLocation, const EPMCoverSearchMode& SearchMode);

	// Once cover actors are sorted, iterate until we find one with a valid cover spot
	static class UPMCoverSpot* GetBestCoverSpot(const TArray<AActor*>& CoverActors, const FVector& TargetLocation, const AActor* Querier, const bool bTestCoverSpotNavigable = true);
};
