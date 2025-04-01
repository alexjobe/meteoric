// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_ActivateAbility_Latent.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMPuppetComponent.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"
#include "Utils/PuppetMasterUtils.h"

EStateTreeRunStatus FPMSTTask_ActivateAbility_Latent::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
	if (!InstanceData.AIController || !ensure(World))
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::EnterState -- AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TimeStarted = World->GetTimeSeconds();

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FPMSTTask_ActivateAbility_Latent::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
	if (!InstanceData.AIController || !ensure(World))
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::Tick -- AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(InstanceData.AIController);
	UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr;
	if (!PuppetComponent)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::Tick -- Owner must implement IPuppetMasterInterface!"));
		return EStateTreeRunStatus::Failed;
	}

	const float TimeSeconds = World->GetTimeSeconds();
	const float ElapsedTime = TimeSeconds - InstanceData.TimeStarted;

	if (InstanceData.CompletionPolicy == EPMAbilityCompletionPolicy::Duration && ElapsedTime >= InstanceData.Duration)
	{
		if (InstanceData.bAbilityActive)
		{
			PuppetComponent->FinishAbilityByTag(InstanceData.AbilityTag);
		}
		else
		{
			UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::Tick -- Task finished before ability started! "
			"Make sure duration is long enough. AbilityTag: %s"), *InstanceData.AbilityTag.ToString());
		}
		return EStateTreeRunStatus::Succeeded;
	}

	if (InstanceData.CompletionPolicy == EPMAbilityCompletionPolicy::OnAbilityEnd && InstanceData.bAbilityActive)
	{
		if (!InstanceData.AbilitySpecHandle.IsValid())
		{
			return EStateTreeRunStatus::Stopped;
		}
		
		if (!UPuppetMasterUtils::IsAbilityActive(InstanceData.AbilitySpecHandle, *InstanceData.AIController))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	if (InstanceData.ActivationPolicy == EPMAbilityActivationPolicy::OnInputTriggered || !InstanceData.bAbilityActive)
	{
		InstanceData.AbilitySpecHandle = PuppetComponent->ActivateAbilityByTag(InstanceData.AbilityTag, InstanceData.ActivationPolicy);
		InstanceData.bAbilityActive = true;
	}

	return EStateTreeRunStatus::Running;
}

void FPMSTTask_ActivateAbility_Latent::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(InstanceData.AIController);
	if (UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr; ensure(PuppetComponent))
	{
		PuppetComponent->FinishAbilityByTag(InstanceData.AbilityTag);
	}
	else
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::ExitState -- Owner must implement IPuppetMasterInterface!"));
	}
}
