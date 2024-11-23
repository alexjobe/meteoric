// Copyright Alex Jobe


#include "METPlayerState.h"

#include "Meteoric/GAS/METAbilitySystemComponent.h"

AMETPlayerState::AMETPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMETAbilitySystemComponent>("AbilitySystemComponent");
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->SetIsReplicated(true);
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	}

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AMETPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
