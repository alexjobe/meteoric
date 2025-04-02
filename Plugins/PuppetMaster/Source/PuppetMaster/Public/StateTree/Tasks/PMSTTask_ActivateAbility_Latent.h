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

	/** Delay before the task ends. */
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "!bRunForever", ClampMin="0.0"))
	float Duration = 1.f;

	/** Adds random range to the Duration. */
	UPROPERTY(EditAnywhere, Category = Parameter, meta = (EditCondition = "!bRunForever", ClampMin="0.0"))
	float RandomDeviation = 0.f;

	/** If true the task will run forever until a transition stops it. */
	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bRunForever = false;

	/** Internal countdown in seconds. */
	float RemainingTime = 0.f;

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

#if WITH_EDITOR
	//~ Begin FStateTreeTaskCommonBase interface
	virtual FText GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting = EStateTreeNodeFormatting::Text) const override;
	//~ End FStateTreeTaskCommonBase interface
#endif // WITH_EDITOR

private:
	bool TryFinishAbility(const FStateTreeExecutionContext& Context, const float DeltaTime, class UPMPuppetComponent& PuppetComponent) const;
};
