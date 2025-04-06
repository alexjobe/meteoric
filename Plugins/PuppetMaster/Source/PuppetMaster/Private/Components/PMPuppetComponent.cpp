// Copyright Alex Jobe


#include "Components/PMPuppetComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "GameplayBehaviorsBlueprintFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Logging/PuppetMasterLog.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Touch.h"

UPMPuppetComponent::UPMPuppetComponent()
	: MinIdealTargetDistance(500.f)
	, MaxIdealTargetDistance(2000.f)
	, StateTagKeyName("StateTag")
	, TargetActorKeyName("TargetActor")
	, MinIdealTargetDistanceKeyName("MinIdealTargetDistance")
	, MaxIdealTargetDistanceKeyName("MaxIdealTargetDistance")
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMPuppetComponent::InitializePuppet(AAIController* InController, const FGameplayTag& InState)
{
	Controller = InController;
	ensure(Controller);

	PerceptionComponent = Controller->GetPerceptionComponent();
	BlackboardComponent = Controller->GetBlackboardComponent();
	StateTreeAIComponent = Controller->GetComponentByClass<UStateTreeAIComponent>();

	SetState(InState);
	SetIdealTargetDistance(MinIdealTargetDistance, MaxIdealTargetDistance);

	if (ensure(PerceptionComponent))
	{
		PerceptionComponent->OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::PerceptionComponent_OnTargetPerceptionInfoUpdated);
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

	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->SendStateTreeEvent(FStateTreeEvent(TargetActorUpdatedEventTag));
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

	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->SendStateTreeEvent(FStateTreeEvent(TargetActorUpdatedEventTag));
	}
}

void UPMPuppetComponent::SetIdealTargetDistance(float InMinIdealTargetDistance, float InMaxIdealTargetDistance)
{
	MinIdealTargetDistance = InMinIdealTargetDistance;
	MaxIdealTargetDistance = InMaxIdealTargetDistance;

	if (ensure(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsFloat(MinIdealTargetDistanceKeyName, MinIdealTargetDistance);
		BlackboardComponent->SetValueAsFloat(MaxIdealTargetDistanceKeyName, MaxIdealTargetDistance);
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

void UPMPuppetComponent::PerceptionComponent_OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	if (!ensure(PerceptionComponent)) return;

	AActor* PerceivedActor = UpdateInfo.Target.IsValid() ? UpdateInfo.Target.Get() : nullptr;
	if (!PerceivedActor) return;
	
	const UAISenseConfig* SenseConfig = PerceptionComponent->GetSenseConfig(UpdateInfo.Stimulus.Type);
	if (!ensure(SenseConfig)) return;

	if (SenseConfig->GetClass() == UAISenseConfig_Sight::StaticClass())
	{
		HandleSense_Sight(*PerceivedActor, UpdateInfo.Stimulus);
	}
	else if (SenseConfig->GetClass() == UAISenseConfig_Hearing::StaticClass())
	{
		HandleSense_Hearing(*PerceivedActor, UpdateInfo.Stimulus);
	}
	else if (SenseConfig->GetClass() == UAISenseConfig_Damage::StaticClass())
	{
		HandleSense_Damage(*PerceivedActor, UpdateInfo.Stimulus);
	}
	else if (SenseConfig->GetClass() == UAISenseConfig_Touch::StaticClass())
	{
		HandleSense_Touch(*PerceivedActor, UpdateInfo.Stimulus);
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
