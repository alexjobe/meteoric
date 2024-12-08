// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "METHUD.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetHealth(float InNewValue);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxHealth(float InNewValue);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetArmor(float InNewValue);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetMaxArmor(float InNewValue);

	void SetAbilitySystemComponent(class UMETAbilitySystemComponent* InASC);

private:
	UPROPERTY(Transient)
	TObjectPtr<UMETAbilitySystemComponent> AbilitySystemComponent;

	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data);
	void MaxHealthChanged(const FOnAttributeChangeData& Data);
	void ArmorChanged(const FOnAttributeChangeData& Data);
	void MaxArmorChanged(const FOnAttributeChangeData& Data);
	
};
