// Copyright Alex Jobe


#include "METPuppetComponent.h"

#include "AbilitySystemGlobals.h"
#include "METAIController.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"


UMETPuppetComponent::UMETPuppetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	TargetActorUpdatedEventTag = METGameplayTags::AIEvent_TargetActorUpdated;
}

void UMETPuppetComponent::HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus)
{
	if (!TargetActor)
	{
		SetTargetActor(&InActor);
	}
}

void UMETPuppetComponent::HandleSense_Hearing(AActor& InActor, const FAIStimulus& InStimulus)
{
}

void UMETPuppetComponent::HandleSense_Damage(AActor& InActor, const FAIStimulus& InStimulus)
{
	if (!TargetActor)
	{
		SetTargetActor(&InActor);
	}
}

void UMETPuppetComponent::HandleSense_Touch(AActor& InActor, const FAIStimulus& InStimulus)
{
	
}

FGameplayAbilitySpecHandle UMETPuppetComponent::ActivateAbilityByTag(const FGameplayTag& InTag, const EPMAbilityActivationPolicy& InActivationPolicy)
{
	FGameplayAbilitySpecHandle Handle;
	const AAIController* AIController = GetController();
	if (!ensure(AIController)) return Handle;
	
	UMETAbilitySystemComponent* ASC = UMETAbilitySystemUtils::GetMetAbilitySystemComponentFromActor(AIController->GetPawn());
	if (!ensure(ASC)) return Handle;

	switch (InActivationPolicy)
	{
	case EPMAbilityActivationPolicy::OnInputStarted:
		Handle = ASC->Input_AbilityInputStarted(InTag);
		break;
	case EPMAbilityActivationPolicy::OnInputTriggered:
		Handle = ASC->Input_AbilityInputTriggered(InTag);
		break;
	default:
		UE_LOG(LogMET, Warning, TEXT("UMETPuppetComponent::ActivateAbilityByTag -- Activation policy not recognized! %s"), *UEnum::GetValueAsString(InActivationPolicy));
	}
	
	return Handle;
}

void UMETPuppetComponent::FinishAbilityByTag(const FGameplayTag& InTag)
{
	const AAIController* AIController = GetController();
	if (!ensure(AIController)) return;

	if (UMETAbilitySystemComponent* ASC = UMETAbilitySystemUtils::GetMetAbilitySystemComponentFromActor(AIController->GetPawn()); ensure(ASC))
	{
		ASC->Input_AbilityInputCompleted(InTag);	
	}
}

void UMETPuppetComponent::TargetActor_OnGameplayTagEvent(FGameplayTag InTag, int32 InCount)
{
	if (InTag == METGameplayTags::State_Dead && InCount > 0)
	{
		ClearTargetActor();
	}
}
