// Copyright Alex Jobe


#include "METAICharacter.h"

#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"

AMETAICharacter::AMETAICharacter()
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
}

void AMETAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	if (HasAuthority())
	{
		AddCharacterAbilities();
	}
}

void AMETAICharacter::InitAbilityActorInfo()
{
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		if (HasAuthority())
		{
			InitializeDefaultAttributes();
		}
	}
}
