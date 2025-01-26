// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "METWeaponTypes.h"
#include "METWeapon.generated.h"

UCLASS()
class METEORIC_API AMETWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FGameplayTag WeaponTag;
	
	/* Effect applied to owner on equip, and removed on unequip */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UGameplayEffect> EquippedEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
	FName ParentAttachmentSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float AimDownSightsSpeed;

	/* Right Hand Relative to Sight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ADS")
	FTransform RightHandToSight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing")
	TEnumAsByte<EWeaponFiringMode> FiringMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FMETWeaponAnimationSettings AnimationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	FMETWeaponFXSettings FXSettings;

	bool bStartDropped;

	AMETWeapon();
	
	bool IsEquipped() const { return bEquipped; }
	
	void OnEquipped(ACharacter* InOwningCharacter);
	void OnUnequipped();
	void OnAimDownSights(const bool bInIsAiming) const;
	
	void Drop();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Drop();

	void FinishUnequip();
	
public:
	void SetWeaponDroppedState(bool bInDropped);

	void Fire(bool bInHeld);
	bool CanFire() const;

	float GetFiringRate() const { return FiringRate >= 0.f ? FiringRate : 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Reload")
	void StartReload() const;
	
	UFUNCTION(BlueprintCallable, Category = "Reload")
	void FinishReload(const bool bSuccess) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloadSignature, bool, bIsReloading);
	FOnReloadSignature& OnReload() { return OnReloadEvent; }

	ACharacter* GetOwningCharacter() { return OwningCharacter; }
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
	class UMETRecoilComponent* GetRecoilComponent() const { return RecoilComponent; }
	class UMETWeaponSwayComponent* GetWeaponSwayComponent() const { return WeaponSwayComponent; }
	class UMETWeaponSpreadComponent* GetWeaponSpreadComponent() const { return WeaponSpreadComponent; }
	class UMETProjectileWeaponComponent* GetProjectileWeaponComponent() const {	return ProjectileWeaponComponent; }
	class UMETWeaponAmmoComponent* GetAmmoComponent() const { return AmmoComponent; }
	UAnimSequence* GetCharacterIdleWeaponAnim() const { return AnimationSettings.CharacterIdleWeaponAnim; }
	UAnimMontage* GetCharacterEquipWeaponMontage() const { return AnimationSettings.CharacterEquipWeaponMontage; }

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Handling", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETRecoilComponent> RecoilComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Handling", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponSwayComponent> WeaponSwayComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Handling", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponSpreadComponent> WeaponSpreadComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETProjectileWeaponComponent> ProjectileWeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponAmmoComponent> AmmoComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETInteractableComponent> InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float FiringRate;
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient, Replicated);
	TObjectPtr<ACharacter> OwningCharacter;

	TOptional<FActiveGameplayEffectHandle> ActiveEquippedEffectHandle;

	bool bCanFire;
	bool bEquipped;
	float ElapsedTimeSinceFired;
	float ElapsedTimeSinceDropped;

	UPROPERTY(BlueprintAssignable, Category = "Reload", meta = (AllowPrivateAccess = "true"))
	FOnReloadSignature OnReloadEvent;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnReload(const bool bIsReloading) const;

	void SetWeaponPhysicsEnabled(bool bInEnabled);

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
