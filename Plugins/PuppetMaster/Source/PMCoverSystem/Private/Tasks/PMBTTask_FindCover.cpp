// Copyright Alex Jobe


#include "Tasks/PMBTTask_FindCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "Components/PMCoverSpot.h"
#include "Logging/PMCoverSystemLog.h"
#include "Utils/PMCoverSystemUtils.h"


UPMBTTask_FindCover::UPMBTTask_FindCover()
	: SearchMode(EPMCoverSearchMode::LeastDistanceToQuerier)
	, SearchRadius(1000.f)
	, bTestCoverSpotNavigable(true)
{
	NodeName = "FindCover";
	
	// Accept only actors and vectors
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, TargetKey));

	SearchCenterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, SearchCenterKey), AActor::StaticClass());
	SearchCenterKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, SearchCenterKey));

	// Accept only floats
	MinDistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MinDistanceToTargetKey));
	MaxDistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MaxDistanceToTargetKey));

	// Accept only vector
	MoveToLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MoveToLocationKey));
}

void UPMBTTask_FindCover::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetKey.ResolveSelectedKey(*BBAsset);
		SearchCenterKey.ResolveSelectedKey(*BBAsset);
		MinDistanceToTargetKey.ResolveSelectedKey(*BBAsset);
		MaxDistanceToTargetKey.ResolveSelectedKey(*BBAsset);
		MoveToLocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogCoverSystem, Error, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UPMBTTask_FindCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	if (!ensure(BlackboardComp) || !ensure(Pawn))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMBTTask_FindCover::ExecuteTask -- Owner must have BlackboardComponent! %s"),
			*OwnerComp.GetName())
		return EBTNodeResult::Failed;
	}
	
	FPMFindCoverParams FindCoverParams;
	FindCoverParams.ContextActor = Pawn;
	FindCoverParams.SearchMode = SearchMode;
	FindCoverParams.SearchRadius = SearchRadius;
	FindCoverParams.bTestCoverSpotNavigable = bTestCoverSpotNavigable;
	FindCoverParams.TargetLocation = UPMCoverSystemUtils::GetKeyLocation(BlackboardComp, TargetKey);
	FindCoverParams.SearchCenter = UPMCoverSystemUtils::GetKeyLocation(BlackboardComp, SearchCenterKey);
	FindCoverParams.MinDistanceToTarget = UPMCoverSystemUtils::GetKeyFloatValue(BlackboardComp, MinDistanceToTargetKey);
	FindCoverParams.MaxDistanceToTarget = UPMCoverSystemUtils::GetKeyFloatValue(BlackboardComp, MaxDistanceToTargetKey);
	
	if (const UPMCoverSpot* CoverSpot = UPMCoverSystemUtils::FindCover(FindCoverParams))
	{
		BlackboardComp->SetValueAsVector(MoveToLocationKey.SelectedKeyName, CoverSpot->GetComponentLocation());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
