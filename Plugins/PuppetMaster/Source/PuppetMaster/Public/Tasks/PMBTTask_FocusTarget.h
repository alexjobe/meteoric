// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_FocusTarget.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UPMBTTask_FocusTarget : public UBTTaskNode
{
	GENERATED_BODY()

	PUPPETMASTER_API UPMBTTask_FocusTarget(const FObjectInitializer& ObjectInitializer);

	PUPPETMASTER_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector FocusTargetKey;
};
