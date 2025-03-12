// Copyright Alex Jobe


#include "Decorators/PMBTDecorator_CoverCheck.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Components/PMCoverSpot.h"
#include "Components/PMCoverUserComponent.h"
#include "Logging/PMCoverSystemLog.h"
#include "Utils/PMCoverSystemUtils.h"

UPMBTDecorator_CoverCheck::UPMBTDecorator_CoverCheck()
{
	NodeName = "CoverCheck";
	INIT_DECORATOR_NODE_NOTIFY_FLAGS();

	// Accept only actors and vectors
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTDecorator_CoverCheck, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTDecorator_CoverCheck, TargetKey));
}

void UPMBTDecorator_CoverCheck::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogCoverSystem, Error, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBlackboardNotificationResult UPMBTDecorator_CoverCheck::OnBlackboardKeyValueChange(const UBlackboardComponent& Blackboard, FBlackboard::FKey ChangedKeyID) const
{
	UBehaviorTreeComponent* BehaviorComp = Cast<UBehaviorTreeComponent>(Blackboard.GetBrainComponent());
	if (BehaviorComp == nullptr)
	{
		return EBlackboardNotificationResult::RemoveObserver;
	}

	if (TargetKey.GetSelectedKeyID() == ChangedKeyID)
	{
		BehaviorComp->RequestExecution(this);		
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

bool UPMBTDecorator_CoverCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr;
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!ensure(CoverUserComponent) || !ensure(BlackboardComp))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMBTDecorator_CoverCheck::CalculateRawConditionValue -- Owner must have BlackboardComponent and CoverUserComponent! %s"),
			*OwnerComp.GetName())
		return false;
	}

	const FVector TargetLocation = UPMCoverSystemUtils::GetKeyLocation(BlackboardComp, TargetKey);

	if (const UPMCoverSpot* CoverSpot = CoverUserComponent->GetOccupiedCoverSpot())
	{
		return CoverSpot->GetCoverScore(TargetLocation) > 0.f;
	}

	return false;
}

void UPMBTDecorator_CoverCheck::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		const auto KeyID = TargetKey.GetSelectedKeyID();
		BlackboardComp->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &UPMBTDecorator_CoverCheck::OnBlackboardKeyValueChange));
	}
}

void UPMBTDecorator_CoverCheck::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComp->UnregisterObserversFrom(this);
	}
}
