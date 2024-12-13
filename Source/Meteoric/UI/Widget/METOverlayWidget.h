// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "METOverlayWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, AmmoCount, int32, MaxAmmo);

/**
 * 
 */
UCLASS()
class METEORIC_API UMETOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	explicit UMETOverlayWidget(const FObjectInitializer& ObjectInitializer);
	
	void SetHealth(const float InNewValue);
	void SetMaxHealth(const float InNewValue);
	void SetArmor(const float InNewValue);
	void SetMaxArmor(const float InNewValue);
	void SetWeaponAmmo(const int32 InAmmoCount, const int32 InMaxAmmo);
	void SetReserveAmmo(const int32 InAmmoCount, const int32 InMaxAmmo);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWeaponAmmo() const { return WeaponAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWeaponMaxAmmo() const { return WeaponMaxAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetReserveAmmo() const { return ReserveAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetReserveMaxAmmo() const { return ReserveMaxAmmo; }

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

	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoChangedSignature OnWeaponAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoChangedSignature OnReserveAmmoChanged;

protected:
	float Health;
	float MaxHealth;
	float Armor;
	float MaxArmor;
	int32 WeaponAmmo;
	int32 WeaponMaxAmmo;
	int32 ReserveAmmo;
	int32 ReserveMaxAmmo;
	
};
