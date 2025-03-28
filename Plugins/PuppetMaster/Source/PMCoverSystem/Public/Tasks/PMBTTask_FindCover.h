// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PMCoverSystemTypes.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BTTaskNode.h"
#include "PMBTTask_FindCover.generated.h"

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
};
