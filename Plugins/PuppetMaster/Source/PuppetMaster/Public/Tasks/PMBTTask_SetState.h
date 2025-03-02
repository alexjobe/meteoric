// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_SetState.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UPMBTTask_SetState : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	PUPPETMASTER_API UPMBTTask_SetState();

	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual FString GetStaticDescription() const override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
	FGameplayTag StateTag;
};
