// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PuppetMasterTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_ActivateAbility_Instant.generated.h"

/*
 * Gameplay Abilities that are "fire and forget" i.e. have no duration
 */
UCLASS(MinimalAPI)
class UPMBTTask_ActivateAbility_Instant : public UBTTaskNode
{
	GENERATED_BODY()

public:
	PUPPETMASTER_API UPMBTTask_ActivateAbility_Instant();
	
	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, Category = "Node")
	EPMAbilityActivationPolicy ActivationPolicy;
};
