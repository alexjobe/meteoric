// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "METWeaponTypes.h"
#include "METWeapon.generated.h"

UCLASS()
class METEORIC_API AMETWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	/* Effect applied to owner on equip, and removed on unequip */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UGameplayEffect> EquippedEffectClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing")
	float FiringRate;

	AMETWeapon();

	void OnEquipped(ACharacter* InOwningCharacter);
	void OnUnequipped();
	void OnAimDownSights(bool bInIsAiming) const;
	
	void Drop();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Drop();

	void RemoveOwningCharacter();
	
public:
	void SetWeaponDroppedState(bool bInDropped);

	void Fire(bool bInHeld);
	bool CanFire() const;

	class UMETRecoilComponent* GetRecoilComponent() const { return RecoilComponent; }
	class UMETWeaponSwayComponent* GetWeaponSwayComponent() const { return WeaponSwayComponent; }
	class UMETProjectileWeaponComponent* GetProjectileWeaponComponent() const {	return ProjectileWeaponComponent; }
	UAnimSequence* GetCharacterIdleWeaponAnim() const { return CharacterIdleWeaponAnim; }
	UAnimMontage* GetCharacterEquipWeaponMontage() const { return CharacterEquipWeaponMontage; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETRecoilComponent> RecoilComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponSwayComponent> WeaponSwayComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETProjectileWeaponComponent> ProjectileWeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETInteractableComponent> InteractableComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> CharacterFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> CharacterIdleWeaponAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> CharacterEquipWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> WeaponFireAnim;
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetMesh() const { return Mesh; }

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_OwningCharacter);
	TObjectPtr<ACharacter> OwningCharacter;

	TOptional<FActiveGameplayEffectHandle> EquippedEffectHandle;

	float LastTimeFired;
	float ElapsedTimeSinceDropped;

	UFUNCTION()
	void OnRep_OwningCharacter(ACharacter* InOldOwner);

	void SetWeaponPhysicsEnabled(bool bInEnabled);

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
