// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "METHUD.generated.h"

/**
 * MET HUD
 */
UCLASS()
class METEORIC_API AMETHUD : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Overlay", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UMETOverlayWidget> OverlayWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Overlay", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMETOverlayWidget> OverlayWidget;

public:
	void Initialize(class UMETAbilitySystemComponent* InASC);

protected:
	void SetHealth(const float InNewValue) const;
	void SetMaxHealth(const float InNewValue) const;
	void SetArmor(const float InNewValue) const;
	void SetMaxArmor(const float InNewValue) const;
	
	UPROPERTY(Transient)
	TObjectPtr<UMETAbilitySystemComponent> AbilitySystemComponent;

	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ArmorChanged(const FOnAttributeChangeData& Data) const;
	void MaxArmorChanged(const FOnAttributeChangeData& Data) const;

	UPROPERTY(Transient)
	TObjectPtr<class UMETAmmoDataAsset> EquippedWeaponAmmoType;

	UFUNCTION()
	void AmmoManager_WeaponAmmoChanged(UMETAmmoDataAsset* const AmmoType, int32 AmmoCount, int32 MaxAmmo);

	UFUNCTION()
	void AmmoManager_ReserveAmmoChanged(UMETAmmoDataAsset* const AmmoType, int32 AmmoCount, int32 MaxAmmo);
	
	UFUNCTION()
	void AmmoManager_WeaponAmmoTypeChanged(UMETAmmoDataAsset* const AmmoType, int32 AmmoCount, int32 MaxAmmo);
};
