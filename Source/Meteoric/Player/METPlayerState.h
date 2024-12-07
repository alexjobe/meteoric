// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "METPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMETPlayerState();

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	class UMETAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UMETAttributeSet> AttributeSet;

	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data) const;
};
