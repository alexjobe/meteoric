// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "METOverlayWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoCountChangedSignature, int32, AmmoCount, int32, MaxAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedAmmoTypeChangedSignature, const class UMETAmmoDataAsset* const, AmmoType);

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
	void SetWeaponAmmoCount(const int32 InAmmoCount, const int32 InMaxAmmo);
	void SetReserveAmmoCount(const int32 InAmmoCount, const int32 InMaxAmmo);
	void SetEquippedAmmoType(const UMETAmmoDataAsset* const InAmmoType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxArmor() const { return MaxArmor; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWeaponAmmoCount() const { return WeaponAmmoCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetWeaponMaxAmmoCount() const { return WeaponMaxAmmoCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetReserveAmmoCount() const { return ReserveAmmoCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetReserveMaxAmmoCount() const { return ReserveMaxAmmoCount; }

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
	FOnAmmoCountChangedSignature OnWeaponAmmoCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnAmmoCountChangedSignature OnReserveAmmoCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Ammo")
	FOnEquippedAmmoTypeChangedSignature OnEquippedAmmoTypeChanged;

protected:
	float Health;
	float MaxHealth;
	float Armor;
	float MaxArmor;
	int32 WeaponAmmoCount;
	int32 WeaponMaxAmmoCount;
	int32 ReserveAmmoCount;
	int32 ReserveMaxAmmoCount;
};
