// Copyright Alex Jobe


#include "Tasks/PMBTTask_FocusTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/PuppetMasterLog.h"


UPMBTTask_FocusTarget::UPMBTTask_FocusTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "FocusTarget";
}

void UPMBTTask_FocusTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		FocusTargetKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogPuppetMaster, Error, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UPMBTTask_FocusTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController ? OwnerComp.GetBlackboardComponent() : nullptr;
	if (!ensure(BlackboardComponent)) return EBTNodeResult::Failed;
	
	AActor* FocusActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FocusTargetKey.SelectedKeyName));
	if (!FocusActor) return EBTNodeResult::Failed;

	AIController->SetFocus(FocusActor);
	return EBTNodeResult::Succeeded;
}
