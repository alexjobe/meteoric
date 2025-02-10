// Copyright Alex Jobe


#include "METPuppetComponent.h"

#include "Meteoric/METGameplayTags.h"


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