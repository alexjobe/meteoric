// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METGameplayAbility.h"
#include "METAbility_FireWeapon.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETAbility_FireWeapon : public UMETGameplayAbility
{
	GENERATED_BODY()

public:
	UMETAbility_FireWeapon();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};