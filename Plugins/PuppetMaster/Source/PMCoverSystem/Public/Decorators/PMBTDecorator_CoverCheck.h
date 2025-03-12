// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "PMBTDecorator_CoverCheck.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UPMBTDecorator_CoverCheck : public UBTDecorator
{
	GENERATED_BODY()

public:
	UPMBTDecorator_CoverCheck();
	
	//~ Begin UBTDecorator interface
	PMCOVERSYSTEM_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	//~ End UBTDecorator interface

	PMCOVERSYSTEM_API EBlackboardNotificationResult OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard") 
	FBlackboardKeySelector TargetKey;

	//~ Begin UBTDecorator interface
	PMCOVERSYSTEM_API virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	PMCOVERSYSTEM_API virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PMCOVERSYSTEM_API virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTDecorator interface
};
