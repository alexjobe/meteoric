// Copyright Alex Jobe


#include "METPuppetComponent.h"

#include "AbilitySystemGlobals.h"
#include "METAIController.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"


UMETPuppetComponent::UMETPuppetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETPuppetComponent::HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus)
{
	if (!FocusTarget)
	{
		SetFocusTarget(&InActor);
		SetState(METGameplayTags::AIState_Attacking);
	}
}

void UMETPuppetComponent::HandleSense_Hearing(AActor& InActor, const FAIStimulus& InStimulus)
{
}

void UMETPuppetComponent::ActivateAbilityByTag(const FGameplayTag& InTag, const bool bInHeld)
{
	const AAIController* AIController = GetController();
	if (!ensure(AIController)) return;
	
	UMETAbilitySystemComponent* ASC = UMETAbilitySystemUtils::GetMetAbilitySystemComponentFromActor(AIController->GetPawn());
	if (!ensure(ASC)) return;

	if (bInHeld)
	{
		ASC->Input_AbilityInputTagHeld(InTag);
	}
	else
	{
		ASC->Input_AbilityInputTagPressed(InTag);
	}
}

void UMETPuppetComponent::DeactivateAbilityByTag(const FGameplayTag& InTag)
{
	const AAIController* AIController = GetController();
	if (!ensure(AIController)) return;

	if (UMETAbilitySystemComponent* ASC = UMETAbilitySystemUtils::GetMetAbilitySystemComponentFromActor(AIController->GetPawn()); ensure(ASC))
	{
		ASC->Input_AbilityInputTagReleased(InTag);	
	}
}

void UMETPuppetComponent::FocusTarget_OnGameplayTagEvent(FGameplayTag InTag, int32 InCount)
{
	if (InTag == METGameplayTags::State_Dead && InCount > 0)
	{
		ClearFocusTarget();
	}
}
