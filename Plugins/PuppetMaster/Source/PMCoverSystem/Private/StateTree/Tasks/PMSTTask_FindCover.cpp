// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_FindCover.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMCoverSpot.h"
#include "Logging/PMCoverSystemLog.h"
#include "Utils/PMCoverSystemUtils.h"

FPMSTTask_FindCover::FPMSTTask_FindCover()
{
	bShouldCallTick = false;
}

EStateTreeRunStatus FPMSTTask_FindCover::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	APawn* Pawn = InstanceData.AIController ? InstanceData.AIController->GetPawn() : nullptr;
	if (!Pawn)
	{
		UE_VLOG(Context.GetOwner(), LogCoverSystem, Error, TEXT("FPMSTTask_FindCover failed -- pawn not found!"));
		return EStateTreeRunStatus::Failed;
	}

	FPMFindCoverParams FindCoverParams;
	FindCoverParams.ContextActor = Pawn;
	FindCoverParams.SearchMode = InstanceData.SearchMode;
	FindCoverParams.SearchRadius = InstanceData.SearchRadius;
	FindCoverParams.bTestCoverSpotNavigable = InstanceData.bTestCoverSpotNavigable;
	FindCoverParams.TargetLocation = InstanceData.TargetLocation;
	FindCoverParams.SearchCenter = InstanceData.SearchCenter;
	FindCoverParams.MinDistanceToTarget = InstanceData.MinDistanceToTarget;
	FindCoverParams.MaxDistanceToTarget = InstanceData.MaxDistanceToTarget;

	const UPMCoverSpot* CoverSpot = UPMCoverSystemUtils::FindCover(FindCoverParams);
	if (!CoverSpot)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.FoundCoverLocation = CoverSpot->GetComponentLocation();
	return EStateTreeRunStatus::Running;
}
