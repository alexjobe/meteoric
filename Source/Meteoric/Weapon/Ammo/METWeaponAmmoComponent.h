// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METAmmoDataAsset.h"
#include "Components/ActorComponent.h"
#include "METWeaponAmmoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo", meta = (ClampMin = 0))
	int32 MaxAmmo;
	
	UMETWeaponAmmoComponent();

	virtual void BeginPlay() override;
	
	int32 Reload();

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void CycleAmmo(const bool bInForward = true);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetAmmoCount() const { return AmmoCount; }

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetMaxAmmo() const { return MaxAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	float GetAmmoPercentage() const;

	UMETAmmoDataAsset* GetCurrentAmmoType() const { return CurrentAmmoType; }

	const FMETAmmoDamageConfig* GetImpactDamageConfig() const;
	const FMETAmmoDamageConfig* GetDelayedDamageConfig() const;

	// Try to consume ammo, and return true if successful
	bool TryConsumeAmmo(const int32 InConsumeCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void FillReserveAmmo();

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnWeaponUnequipped();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETAmmoDataAsset> CurrentAmmoType;

	UPROPERTY(Replicated)
	int32 AmmoCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<UMETAmmoDataAsset>> AmmoTypes;

	int SelectedAmmoSlot;

private:
	UPROPERTY(ReplicatedUsing = OnRep_OwningCharacter)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient)
	TObjectPtr<class UMETAmmoManager> AmmoManager;

	UFUNCTION()
	void OnRep_OwningCharacter(ACharacter* InOldOwner);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
