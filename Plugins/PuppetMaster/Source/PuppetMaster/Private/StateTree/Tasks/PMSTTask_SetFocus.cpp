// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_SetFocus.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Logging/PuppetMasterLog.h"

EStateTreeRunStatus FPMSTTask_SetFocus::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.AIController)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_SetFocus failed since AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.AIController->SetFocus(InstanceData.TargetActor);
	return EStateTreeRunStatus::Running;
}

void FPMSTTask_SetFocus::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.AIController)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_SetFocus::ExitState -- AIController is missing."));
		return;
	}

	InstanceData.AIController->ClearFocus(EAIFocusPriority::Gameplay);
}
