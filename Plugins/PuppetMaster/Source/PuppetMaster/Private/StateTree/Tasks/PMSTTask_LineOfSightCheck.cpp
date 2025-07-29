// Copyright Alex Jobe


#include "StateTree/Tasks/PMSTTask_LineOfSightCheck.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Logging/PuppetMasterLog.h"

EStateTreeRunStatus FPMSTTask_LineOfSightCheck::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	return FStateTreeTaskCommonBase::EnterState(Context, Transition);
}

EStateTreeRunStatus FPMSTTask_LineOfSightCheck::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
	if (!InstanceData.AIController || !ensure(World))
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_LineOfSightCheck::Tick-- AIController is missing."));
		return EStateTreeRunStatus::Failed;
	}

	const APawn* MyPawn = InstanceData.AIController->GetPawn();

	if (!MyPawn || !InstanceData.TargetActor)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTTask_LineOfSightCheck::Tick -- Pawn is missing."));
		return EStateTreeRunStatus::Failed;
	}

	const APawn* TargetPawn = Cast<APawn>(InstanceData.TargetActor);
	const FVector TargetLocation = TargetPawn ? TargetPawn->GetNavAgentLocation() : InstanceData.TargetActor->GetActorLocation();
	
	const FVector TraceStart = MyPawn->GetNavAgentLocation() + FVector(0, 0, InstanceData.QuerierHeightOffset);
	const FVector TraceEnd = TargetLocation + FVector(0, 0, InstanceData.TargetHeightOffset);

	//DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, -1.f, 0, 1.f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(MyPawn);
	World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, InstanceData.TraceChannel, CollisionParams);
	const bool bHasLineOfSight = !HitResult.IsValidBlockingHit();

	return bHasLineOfSight ? EStateTreeRunStatus::Running : EStateTreeRunStatus::Failed;
}
