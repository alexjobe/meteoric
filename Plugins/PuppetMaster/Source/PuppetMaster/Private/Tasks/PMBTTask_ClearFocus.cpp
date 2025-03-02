// Copyright Alex Jobe


#include "Tasks/PMBTTask_ClearFocus.h"

#include "AIController.h"

UPMBTTask_ClearFocus::UPMBTTask_ClearFocus()
{
	NodeName = "ClearFocus";
}

EBTNodeResult::Type UPMBTTask_ClearFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}
