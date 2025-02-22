// Copyright Alex Jobe


#include "Tasks/PMBTTask_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/PuppetMasterLog.h"


UPMBTTask_SetFocus::UPMBTTask_SetFocus(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "SetFocus";
}

void UPMBTTask_SetFocus::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		FocusTargetKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogPuppetMaster, Error, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UPMBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BlackboardComponent = AIController ? OwnerComp.GetBlackboardComponent() : nullptr;
	if (!ensure(BlackboardComponent)) return EBTNodeResult::Failed;
	
	AActor* FocusActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(FocusTargetKey.SelectedKeyName));
	if (!FocusActor) return EBTNodeResult::Failed;

	AIController->SetFocus(FocusActor);
	return EBTNodeResult::Succeeded;
}
