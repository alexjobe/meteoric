// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "METAbilityTypes.h"
#include "METAbilitySystemComponent.generated.h"

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
	
	FGameplayAbilitySpecHandle Input_AbilityInputStarted(const FGameplayTag& InputTag);
	FGameplayAbilitySpecHandle Input_AbilityInputTriggered(const FGameplayTag& InputTag);
	void Input_AbilityInputCompleted(const FGameplayTag& InputTag);

	bool bCharacterAbilitiesGiven;

protected:
	//~ Begin UAbilitySystemComponent interface
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	//~ End UAbilitySystemComponent interface

private:
	FGameplayAbilitySpecHandle ActivateAbility(const FGameplayTag& InputTag, const EMETAbilityActivationPolicy& InActivationPolicy);
};
