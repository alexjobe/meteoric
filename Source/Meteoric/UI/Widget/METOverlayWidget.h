// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "METOverlayWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

/**
 * 
 */
UCLASS()
class METEORIC_API UMETOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMETOverlayWidget(const FObjectInitializer& ObjectInitializer);
	
	void SetHealth(float InNewValue);
	void SetMaxHealth(float InNewValue);
	void SetArmor(float InNewValue);
	void SetMaxArmor(float InNewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealthPercentage() const { return (MaxHealth != 0.0f) ? (Health / MaxHealth) : 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetArmorPercentage() const { return (MaxArmor != 0.0f) ? (Armor / MaxArmor) : 0.0f; }

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeValueChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeValueChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeValueChangedSignature OnArmorChanged;

	UPROPERTY(BlueprintAssignable, Category = "Attributes")
	FOnAttributeValueChangedSignature OnMaxArmorChanged;

protected:
	float Health;
	float MaxHealth;
	float Armor;
	float MaxArmor;
};
