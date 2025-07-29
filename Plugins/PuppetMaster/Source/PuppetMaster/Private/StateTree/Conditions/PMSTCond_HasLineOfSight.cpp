// Copyright Alex Jobe


#include "StateTree/Conditions/PMSTCond_HasLineOfSight.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Logging/PuppetMasterLog.h"

bool FPMSTCond_HasLineOfSight::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	const UWorld* World = Context.GetWorld();
    if (!InstanceData.AIController || !ensure(World))
    {
    	UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTCond_HasLineOfSight::TestCondition -- AIController is missing."));
    	return false;
    }

	const APawn* MyPawn = InstanceData.AIController->GetPawn();

	if (!MyPawn || !InstanceData.TargetActor)
	{
		UE_VLOG(Context.GetOwner(), LogPuppetMaster, Error, TEXT("FPMSTCond_HasLineOfSight::TestCondition -- Pawn is missing."));
		return false;
	}

	const APawn* TargetPawn = Cast<APawn>(InstanceData.TargetActor);
	const FVector TargetLocation = TargetPawn ? TargetPawn->GetNavAgentLocation() : InstanceData.TargetActor->GetActorLocation();
	
	const FVector TraceStart = MyPawn->GetNavAgentLocation() + FVector(0, 0, InstanceData.QuerierHeightOffset);
	const FVector TraceEnd = TargetLocation + FVector(0, 0, InstanceData.TargetHeightOffset);

	//DrawDebugLine(World, TraceStart, TraceEnd, FColor::Red, false, 2.f, 0, 1.f);

	FHitResult HitResult;
	World->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, InstanceData.TraceChannel);
	return !HitResult.IsValidBlockingHit();
}
