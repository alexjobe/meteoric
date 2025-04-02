// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_ActivateAbility_Latent.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMPuppetComponent.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"
#include "Utils/PuppetMasterUtils.h"

#define LOCTEXT_NAMESPACE "PuppetMaster"

EStateTreeRunStatus FPMSTTask_ActivateAbility_Latent::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
	if (!InstanceData.AIController || !ensure(World))
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::EnterState -- AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.RemainingTime = FMath::FRandRange(
			FMath::Max(0.0f, InstanceData.Duration - InstanceData.RandomDeviation), (InstanceData.Duration + InstanceData.RandomDeviation));

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

	if (TryFinishAbility(Context, DeltaTime, *PuppetComponent))
	{
		return EStateTreeRunStatus::Succeeded;
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
#if WITH_EDITOR
FText FPMSTTask_ActivateAbility_Latent::GetDescription(const FGuid& ID, FStateTreeDataView InstanceDataView, const IStateTreeBindingLookup& BindingLookup, EStateTreeNodeFormatting Formatting) const
{
	const FInstanceDataType* InstanceData = InstanceDataView.GetPtr<FInstanceDataType>();
	check(InstanceData);

	FText AbilityTagText = BindingLookup.GetBindingSourceDisplayName(FStateTreePropertyPath(ID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, AbilityTag)), Formatting);
	if (AbilityTagText.IsEmpty())
	{
		AbilityTagText = FText::FromString(InstanceData->AbilityTag.ToString());
	}

	if (Formatting == EStateTreeNodeFormatting::RichText)
	{
		return FText::Format(LOCTEXT("ActivateAbilityLatent", "<b>Activate Ability</> {0}"), AbilityTagText);	
	}
	return FText::Format(LOCTEXT("ActivateAbilityLatent", "Activate Ability {0}"), AbilityTagText);
}
#endif // WITH_EDITOR

bool FPMSTTask_ActivateAbility_Latent::TryFinishAbility(const FStateTreeExecutionContext& Context, const float DeltaTime, UPMPuppetComponent& PuppetComponent) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.bRunForever) return false;

	InstanceData.RemainingTime -= DeltaTime;
	
	if (InstanceData.CompletionPolicy == EPMAbilityCompletionPolicy::Duration && InstanceData.RemainingTime <= 0.f)
	{
		if (InstanceData.bAbilityActive)
		{
			PuppetComponent.FinishAbilityByTag(InstanceData.AbilityTag);
		}
		else
		{
			UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_ActivateAbility_Latent::Tick -- Task finished before ability started! "
				"Make sure duration is long enough. AbilityTag: %s"), *InstanceData.AbilityTag.ToString());
		}
		return true;
	}

	if (InstanceData.CompletionPolicy == EPMAbilityCompletionPolicy::OnAbilityEnd && InstanceData.bAbilityActive)
	{
		if (!InstanceData.AbilitySpecHandle.IsValid() || !UPuppetMasterUtils::IsAbilityActive(InstanceData.AbilitySpecHandle, *InstanceData.AIController))
		{
			return true;
		}
	}

	return false;
}
