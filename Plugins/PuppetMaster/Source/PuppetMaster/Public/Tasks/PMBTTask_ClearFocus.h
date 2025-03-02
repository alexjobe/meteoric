// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_ClearFocus.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UPMBTTask_ClearFocus : public UBTTaskNode
{
	GENERATED_BODY()

public:
	PUPPETMASTER_API UPMBTTask_ClearFocus();
	
	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTTaskNode interface
};
