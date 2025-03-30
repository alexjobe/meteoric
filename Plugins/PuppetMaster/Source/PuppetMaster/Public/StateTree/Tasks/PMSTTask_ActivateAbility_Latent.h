// Copyright Alex Jobe

#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "PuppetMasterTypes.h"
#include "StateTreeTaskBase.h"

#include "PMSTTask_ActivateAbility_Latent.generated.h"

USTRUCT()
struct FPMLatentAbilityTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, Category = Parameter)
	EPMAbilityActivationPolicy ActivationPolicy = EPMAbilityActivationPolicy::OnInputStarted;

	/* 
	 * Duration -- The task will stay active for the specified duration
	 * OnAbilityEnd -- The task will end when the ability ends
	 */
	UPROPERTY(EditAnywhere, Category = Parameter)
	EPMAbilityCompletionPolicy CompletionPolicy = EPMAbilityCompletionPolicy::Duration;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float Duration = 5.f;

	float TimeStarted = 0.f;

	bool bAbilityActive = false;

	FGameplayAbilitySpecHandle AbilitySpecHandle;
};

USTRUCT(meta = (DisplayName = "Activate Ability - Latent", Category = "PuppetMaster"))
struct FPMSTTask_ActivateAbility_Latent : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMLatentAbilityTaskInstanceData;

	//~ Begin FStateTreeTaskCommonBase interface
	PUPPETMASTER_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PUPPETMASTER_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	PUPPETMASTER_API virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	PUPPETMASTER_API virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~ End FStateTreeTaskCommonBase interface
};
