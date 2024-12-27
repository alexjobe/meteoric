// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METAmmoManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETAmmoManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UMETAmmoManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ReserveMaxAmmo;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetReserveAmmoCount(class UMETAmmoDataAsset* const InType);

	// Adds ammo and returns new count
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 AddReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InAmmoCount);

	// Returns amount successfully consumed
	int32 TryConsumeReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount = 1);

	void WeaponAmmoChanged(const int32 InAmmoCount, const int32 InMaxAmmo) const;
	void WeaponAmmoTypeChanged(UMETAmmoDataAsset* const InType);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoChangedSignature, UMETAmmoDataAsset* const, AmmoType, int32, AmmoCount, int32, MaxAmmo);
	FAmmoChangedSignature& OnWeaponAmmoChanged() { return WeaponAmmoChangedEvent; }
	FAmmoChangedSignature& OnReserveAmmoChanged() { return ReserveAmmoChangedEvent; }
	FAmmoChangedSignature& OnWeaponAmmoTypeChanged() { return WeaponAmmoTypeChangedEvent; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<UMETAmmoDataAsset> EquippedWeaponAmmoType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UMETAmmoDataAsset>, int32> ReserveAmmoTypeToCount;

	UFUNCTION(Client, Reliable)
	void Client_SetReserveAmmo(UMETAmmoDataAsset* InType, const int32 InAmmoCount, const int32 InMaxAmmo);
	
	UFUNCTION(Client, Reliable)
	void Client_WeaponAmmoChanged(const int32 InAmmoCount, const int32 InMaxAmmo) const;

	UFUNCTION(Client, Reliable)
	void Client_WeaponAmmoTypeChanged(UMETAmmoDataAsset* InType);

private:
	FAmmoChangedSignature WeaponAmmoChangedEvent;
	FAmmoChangedSignature ReserveAmmoChangedEvent;
	FAmmoChangedSignature WeaponAmmoTypeChangedEvent;
};
