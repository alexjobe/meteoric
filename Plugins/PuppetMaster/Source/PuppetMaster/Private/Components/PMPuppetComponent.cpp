// Copyright Alex Jobe


#include "Components/PMPuppetComponent.h"

#include "AIController.h"
#include "Logging/PuppetMasterLog.h"
#include "Perception/AIPerceptionComponent.h"

UPMPuppetComponent::UPMPuppetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMPuppetComponent::InitializePuppet(AAIController* InController)
{
	AIController = InController;
	ensure(AIController);

	PerceptionComponent = AIController->GetPerceptionComponent();

	if (ensure(PerceptionComponent))
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::PerceptionComponent_OnTargetPerceptionUpdated);
	}
	else
	{
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMPuppetComponent: PerceptionComponent not found on AIController %s"), *AIController->GetName());	
	}
}

void UPMPuppetComponent::PerceptionComponent_OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
}
