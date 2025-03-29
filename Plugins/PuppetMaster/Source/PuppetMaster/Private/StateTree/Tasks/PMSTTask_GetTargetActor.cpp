// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_GetTargetActor.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMPuppetComponent.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"

FPMSTTask_GetTargetActor::FPMSTTask_GetTargetActor()
{
	bShouldCallTick = false;
}

EStateTreeRunStatus FPMSTTask_GetTargetActor::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (!InstanceData.AIController)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_GetTargetActor failed since AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(InstanceData.AIController);
	const UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr;
	if (!PuppetComponent)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_GetTargetActor failed since PuppetComponent is missing."));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.TargetActor = PuppetComponent->GetTargetActor();

	return EStateTreeRunStatus::Running;
}
