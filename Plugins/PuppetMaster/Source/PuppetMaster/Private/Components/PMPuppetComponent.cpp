// Copyright Alex Jobe


#include "Components/PMPuppetComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
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
	, TargetActorKeyName("TargetActor")
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

void UPMPuppetComponent::SetTargetActor(AActor* const InActor)
{
	if (TargetActor == InActor) return;
	
	ClearTargetActor();
	
	TargetActor = InActor;
	if (ensure(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsObject(TargetActorKeyName, InActor);
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor); ensure(TargetASC))
	{
		TargetASC->RegisterGenericGameplayTagEvent().AddUObject(this, &ThisClass::TargetActor_OnGameplayTagEvent);
	}
}

void UPMPuppetComponent::ClearTargetActor()
{
	if (TargetActor == nullptr) return;

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor); ensure(TargetASC))
	{
		TargetASC->RegisterGenericGameplayTagEvent().RemoveAll(this);
	}
	
	TargetActor = nullptr;
	if (ensure(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsObject(TargetActorKeyName, nullptr);
	}
}

FGameplayAbilitySpecHandle UPMPuppetComponent::ActivateAbilityByTag(const FGameplayTag& InTag, const EPMAbilityActivationPolicy& InActivationPolicy)
{
	// Empty in base class
	return FGameplayAbilitySpecHandle();
}

void UPMPuppetComponent::FinishAbilityByTag(const FGameplayTag& InTag)
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

void UPMPuppetComponent::TargetActor_OnGameplayTagEvent(FGameplayTag InTag, int32 InCount)
{
	// Empty in base class
}
