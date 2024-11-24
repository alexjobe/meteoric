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
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	}

	AttributeSet = CreateDefaultSubobject<UMETAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AMETPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
