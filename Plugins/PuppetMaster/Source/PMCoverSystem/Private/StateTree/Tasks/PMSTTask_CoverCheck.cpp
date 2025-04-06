// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_CoverCheck.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMCoverSpot.h"
#include "Components/PMCoverUserComponent.h"
#include "Logging/PMCoverSystemLog.h"

EStateTreeRunStatus FPMSTTask_CoverCheck::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return FStateTreeTaskCommonBase::EnterState(Context, Transition);
}

EStateTreeRunStatus FPMSTTask_CoverCheck::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const APawn* Pawn = InstanceData.AIController ? InstanceData.AIController->GetPawn() : nullptr;
	UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr;
	
	if (!CoverUserComponent)
	{
		UE_VLOG(Context.GetOwner(), LogCoverSystem, Error, TEXT("FPMSTTask_CoverCheck::Tick-- CoverUserComponent is missing."));
		return EStateTreeRunStatus::Failed;
	}

	bool bValidCover = false;
	if (const UPMCoverSpot* CoverSpot = CoverUserComponent->GetOccupiedCoverSpot())
	{
		bValidCover = CoverSpot->IsValidCover(InstanceData.TargetLocation);
	}

	return bValidCover ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}