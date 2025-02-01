// Copyright Alex Jobe


#include "METPlayerState.h"

#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"

AMETPlayerState::AMETPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMETAbilitySystemComponent>("AbilitySystemComponent");
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->SetIsReplicated(true);

		// TODO: Currently set to Full because of an engine bug where duration gameplay cues don't replicate correctly
		// https://forums.unrealengine.com/t/gameplay-cue-and-periodic-effect-issue/723075
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}

	AttributeSet = CreateDefaultSubobject<UMETAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AMETPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
