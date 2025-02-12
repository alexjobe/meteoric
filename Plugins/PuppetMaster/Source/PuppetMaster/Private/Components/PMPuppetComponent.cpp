// Copyright Alex Jobe


#include "Components/PMPuppetComponent.h"

#include "AIController.h"
#include "GameplayBehaviorsBlueprintFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Logging/PuppetMasterLog.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"

UPMPuppetComponent::UPMPuppetComponent()
	: StateTagKeyName("StateTag")
	, FocusTargetKeyName("FocusTarget")
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMPuppetComponent::InitializePuppet(AAIController* InController, const FGameplayTag& InState)
{
	Controller = InController;
	ensure(Controller);

	PerceptionComponent = Controller->GetPerceptionComponent();
	BlackboardComponent = Controller->GetBlackboardComponent();

	SetState(InState);

	if (ensure(PerceptionComponent))
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::PerceptionComponent_OnTargetPerceptionUpdated);
	}
	else
	{
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMPuppetComponent: PerceptionComponent not found on AIController %s"), *Controller->GetName());	
	}
}

void UPMPuppetComponent::SetState(const FGameplayTag& InState)
{
	StateTag = InState;

	if (ensure(BlackboardComponent))
	{
		UGameplayBehaviorsBlueprintFunctionLibrary::SetValueAsGameplayTagForBlackboardComp(BlackboardComponent, StateTagKeyName, StateTag.GetSingleTagContainer());
	}
}

void UPMPuppetComponent::SetFocusTarget(AActor* const InActor)
{
	FocusTarget = InActor;
	if (ensure(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsObject(FocusTargetKeyName, InActor);
	}
}

void UPMPuppetComponent::ActivateAbilityByTag(const FGameplayTag& InTag, const bool bInHeld)
{
	// Empty in base class
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
	else if (SenseConfig->GetClass() == UAISenseConfig_Hearing::StaticClass())
	{
		HandleSense_Hearing(*InActor, InStimulus);
	}
	else if (SenseConfig->GetClass() == UAISenseConfig_Damage::StaticClass())
	{
		HandleSense_Damage(*InActor, InStimulus);
	}
	else if (SenseConfig->GetClass() == UAISenseConfig_Touch::StaticClass())
	{
		HandleSense_Touch(*InActor, InStimulus);
	}
}

void UPMPuppetComponent::HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus)
{
	// Empty in base class
}

void UPMPuppetComponent::HandleSense_Hearing(AActor& InActor, const FAIStimulus& InStimulus)
{
	// Empty in base class
}

void UPMPuppetComponent::HandleSense_Damage(AActor& InActor, const FAIStimulus& InStimulus)
{
	// Empty in base class
}

void UPMPuppetComponent::HandleSense_Touch(AActor& InActor, const FAIStimulus& InStimulus)
{
	// Empty in base class
}
