// Copyright Alex Jobe


#include "METGameplayAbility.h"

#include "Meteoric/Character/METCharacter.h"

UMETGameplayAbility::UMETGameplayAbility()
	: ActivationPolicy(OnInputPressed)
{
	
}

AMETCharacter* UMETGameplayAbility::GetMetCharacterFromActorInfo() const
{
	return CastChecked<AMETCharacter>(GetAvatarActorFromActorInfo());
}
