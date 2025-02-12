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

public:
	PUPPETMASTER_API UPMBTTask_FocusTarget(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTTaskNode interface

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector FocusTargetKey;
};
