// Copyright Alex Jobe


#include "../Abilities/METGameplayAbility.h"

#include "Meteoric/Character/METCharacter.h"

UMETGameplayAbility::UMETGameplayAbility()
	: ActivationPolicy(OnInputPressed)
{
	
}

AMETCharacter* UMETGameplayAbility::GetMetCharacterFromActorInfo() const
{
	return CastChecked<AMETCharacter>(GetAvatarActorFromActorInfo());
}
