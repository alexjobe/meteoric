// Copyright Alex Jobe


#include "Tasks/PMBTTask_ActivateAbility_Instant.h"

#include "AIController.h"
#include "Components/PMPuppetComponent.h"
#include "Logging/PuppetMasterLog.h"

UPMBTTask_ActivateAbility_Instant::UPMBTTask_ActivateAbility_Instant(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, ActivationPolicy(EPMAbilityActivationPolicy::OnInputStarted)
{
	NodeName = "ActivateAbility_Instant";
}

EBTNodeResult::Type UPMBTTask_ActivateAbility_Instant::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;    
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	UPMPuppetComponent* PuppetComponent = AIController ? AIController->FindComponentByClass<UPMPuppetComponent>() : nullptr;
	if (!ensure(PuppetComponent))
	{
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Instant::ExecuteTask -- PuppetComponent not found! Owner: %s"), *OwnerString);

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return EBTNodeResult::Failed;
	}

	PuppetComponent->ActivateAbilityByTag(AbilityTag, ActivationPolicy);

	return EBTNodeResult::Succeeded;
}
