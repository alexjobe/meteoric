// Copyright Alex Jobe


#include "Tasks/PMBTTask_SetState.h"

#include "AIController.h"
#include "Components/PMPuppetComponent.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"


UPMBTTask_SetState::UPMBTTask_SetState()
{
	NodeName = StateTag.IsValid() ? "SetState: " + StateTag.ToString() : "SetState: Invalid";
}

EBTNodeResult::Type UPMBTTask_SetState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(OwnerComp.GetAIOwner());
	if (UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr; ensure(PuppetComponent))
	{
		PuppetComponent->SetState(StateTag);
		return EBTNodeResult::Succeeded;
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
	UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_SetState::ExecuteTask -- Owner must implement IPuppetMasterInterface! Owner: %s"), *OwnerString);

	return EBTNodeResult::Failed;
}

void UPMBTTask_SetState::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UPMBTTask_SetState, StateTag))
	{
		NodeName = StateTag.IsValid() ? "SetState: " + StateTag.ToString() : "SetState: Invalid";
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FString UPMBTTask_SetState::GetStaticDescription() const
{
	return StateTag.IsValid() ? StateTag.ToString() : "Invalid State";
}