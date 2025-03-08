// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "PuppetMasterTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_ActivateAbility_Latent.generated.h"

UENUM(BlueprintType)
enum class EPMTaskNodeCompletionPolicy : uint8
{
	Duration,
	OnAbilityEnd
};

struct FBTActivateAbilityMemory
{
	float Duration;
	float TimeStarted;
	bool bAbilityStarted;
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	FBTActivateAbilityMemory()
		: Duration(0.f)
		, TimeStarted(0.f)
		, bAbilityStarted(false)
	{}
};

/*
 * Gameplay Abilities that are activated and "held" for a duration, rather than fired off instantly
 * (e.g. holding the trigger on an automatic weapon)
 */
UCLASS(MinimalAPI)
class UPMBTTask_ActivateAbility_Latent : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	PUPPETMASTER_API UPMBTTask_ActivateAbility_Latent();

	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual uint16 GetInstanceMemorySize() const override;
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PUPPETMASTER_API virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PUPPETMASTER_API virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	PUPPETMASTER_API virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	PUPPETMASTER_API virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, Category = "Node")
	EPMAbilityActivationPolicy ActivationPolicy;

	/* 
	 * Duration -- The task node will stay active for the specified duration
	 * OnAbilityEnd -- The task node will end when the ability ends
	 */
	UPROPERTY(EditAnywhere, Category = "Node")
	EPMTaskNodeCompletionPolicy CompletionPolicy;

	UPROPERTY(EditAnywhere, Category = "Node")
	float Duration;

protected:
	PUPPETMASTER_API static bool IsAbilityActive(const FGameplayAbilitySpecHandle& InHandle, const AAIController& InController);
};
