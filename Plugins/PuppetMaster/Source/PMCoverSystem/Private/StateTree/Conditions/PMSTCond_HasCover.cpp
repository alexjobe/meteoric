// Copyright Alex Jobe


#include "StateTree/Conditions/PMSTCond_HasCover.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Components/PMCoverSpot.h"
#include "Components/PMCoverUserComponent.h"
#include "Logging/PMCoverSystemLog.h"

bool FPMSTCond_HasCover::TestCondition(FStateTreeExecutionContext& Context) const
{
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	
	const APawn* Pawn = InstanceData.AIController ? InstanceData.AIController->GetPawn() : nullptr;
	UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr;
	
	if (!CoverUserComponent)
	{
		UE_VLOG(Context.GetOwner(), LogCoverSystem, Error, TEXT("FPMSTCond_HasCover::TestCondition -- CoverUserComponent is missing."));
		return false;
	}

	const UPMCoverSpot* CoverSpot = CoverUserComponent->GetOccupiedCoverSpot();
	return CoverSpot ? CoverSpot->IsValidCover(InstanceData.TargetLocation) : false;
}
