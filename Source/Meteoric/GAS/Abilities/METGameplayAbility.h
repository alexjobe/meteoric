// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "METGameplayAbility.generated.h"

UENUM(BlueprintType)
enum EMETAbilityActivationPolicy : uint8
{
	OnInputPressed,
	OnInputHeld
};

UCLASS()
class METEORIC_API UMETGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMETGameplayAbility();
	
	EMETAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	FGameplayTag GetInputTag() const { return InputTag; }

	UFUNCTION(BlueprintPure, Category = "Ability")
	class AMETCharacter* GetMetCharacterFromActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Activation")
	TEnumAsByte<EMETAbilityActivationPolicy> ActivationPolicy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag InputTag;
};
