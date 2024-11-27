// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "METAbilitySystemComponent.generated.h"

enum EMETAbilityActivationPolicy : uint8;
/**
 * 
 */
UCLASS()
class METEORIC_API UMETAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities);
	
	void Input_AbilityInputTagPressed(const FGameplayTag& InputTag);
	void Input_AbilityInputTagHeld(const FGameplayTag& InputTag);
	void Input_AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	//~ Begin UAbilitySystemComponent interface
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	//~ End UAbilitySystemComponent interface

private:
	void ActivateAbility(const FGameplayTag& InputTag, const EMETAbilityActivationPolicy& InActivationPolicy);
};
