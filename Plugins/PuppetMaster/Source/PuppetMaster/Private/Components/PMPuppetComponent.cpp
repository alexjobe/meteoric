// Copyright Alex Jobe


#include "Components/PMPuppetComponent.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/PuppetMasterLog.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"

UPMPuppetComponent::UPMPuppetComponent()
	: FocusTargetKeyName("FocusTarget")
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMPuppetComponent::InitializePuppet(AAIController* InController)
{
	AIController = InController;
	ensure(AIController);

	PerceptionComponent = AIController->GetPerceptionComponent();
	BlackboardComponent = AIController->GetBlackboardComponent();

	if (ensure(PerceptionComponent))
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::PerceptionComponent_OnTargetPerceptionUpdated);
	}
	else
	{
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMPuppetComponent: PerceptionComponent not found on AIController %s"), *AIController->GetName());	
	}
}

void UPMPuppetComponent::PerceptionComponent_OnTargetPerceptionUpdated(AActor* InActor, FAIStimulus InStimulus)
{
	if (!ensure(PerceptionComponent) || !ensure(InActor)) return;
	
	const UAISenseConfig* SenseConfig = PerceptionComponent->GetSenseConfig(InStimulus.Type);
	if (!ensure(SenseConfig)) return;

	if (SenseConfig->GetClass() == UAISenseConfig_Sight::StaticClass())
	{
		HandleSense_Sight(*InActor, InStimulus);
	}
}

void UPMPuppetComponent::HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus)
{
	FocusTarget = &InActor;

	if (ensure(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsObject(FocusTargetKeyName, FocusTarget);
	}
	
}
