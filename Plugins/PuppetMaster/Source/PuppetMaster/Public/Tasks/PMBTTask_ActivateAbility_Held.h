// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PuppetMasterTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_ActivateAbility_Held.generated.h"

struct FBTActivateAbilityMemory
{
	float Duration;
	float TimeStarted;

	FBTActivateAbilityMemory()
		: Duration(0.f)
		, TimeStarted(0.f)
	{}
};

/*
 * Gameplay Abilities that are activated and "held" for a duration, rather than fired off instantly
 * (e.g. holding the trigger on an automatic weapon)
 */
UCLASS(MinimalAPI)
class UPMBTTask_ActivateAbility_Held : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	PUPPETMASTER_API UPMBTTask_ActivateAbility_Held(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	PUPPETMASTER_API virtual uint16 GetInstanceMemorySize() const override;
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PUPPETMASTER_API virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PUPPETMASTER_API virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag AbilityTag;
	
	UPROPERTY(EditAnywhere, Category = "Node")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Node")
	EPMAbilityActivationPolicy ActivationPolicy;
};
