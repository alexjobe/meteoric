// Copyright Alex Jobe


#include "Tasks/PMBTTask_SetFocus.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/PuppetMasterLog.h"


UPMBTTask_SetFocus::UPMBTTask_SetFocus()
{
	NodeName = "SetFocus";
}

void UPMBTTask_SetFocus::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
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
	
	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	AIController->SetFocus(TargetActor);
	return EBTNodeResult::Succeeded;
}
