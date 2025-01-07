// Copyright Alex Jobe


#include "METPlayerState.h"

#include "Meteoric/METGameplayTags.h"
#include "Meteoric/Character/METPlayerCharacter.h"
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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AMETPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMETPlayerState::HealthChanged(const FOnAttributeChangeData& Data) const
{
	float Health = Data.NewValue;

	AMETPlayerCharacter* Character = Cast<AMETPlayerCharacter>(GetPawn());

	if (Health <= 0.0f && !AbilitySystemComponent->HasMatchingGameplayTag(METGameplayTags::State_Dead))
	{
		if (Character)
		{
			Character->Die();
		}
	}
}