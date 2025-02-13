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
	UMETAbilitySystemComponent();
	
	void AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities, UObject* InSourceObject);
	
	void Input_AbilityInputStarted(const FGameplayTag& InputTag);
	void Input_AbilityInputTriggered(const FGameplayTag& InputTag);
	void Input_AbilityInputCompleted(const FGameplayTag& InputTag);

	bool bCharacterAbilitiesGiven;

protected:
	//~ Begin UAbilitySystemComponent interface
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	//~ End UAbilitySystemComponent interface

private:
	void ActivateAbility(const FGameplayTag& InputTag, const EMETAbilityActivationPolicy& InActivationPolicy);
};
