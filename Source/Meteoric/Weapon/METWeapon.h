// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "METWeaponTypes.h"
#include "METWeapon.generated.h"


UCLASS()
class METEORIC_API AMETWeapon : public AActor
{
	GENERATED_BODY()
	
public:
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
	
public:

	void SetWeaponDropped(bool bInDropped);

	void Fire(bool bInHeld);
	bool CanFire() const;

	class UMETRecoilComponent* GetRecoilComponent() const { return RecoilComponent; }
	class UMETWeaponSwayComponent* GetWeaponSwayComponent() const { return WeaponSwayComponent; }
	UAnimSequence* GetCharacterIdleWeaponAnim() const { return CharacterIdleWeaponAnim; }
	UAnimMontage* GetCharacterEquipWeaponMontage() const { return CharacterEquipWeaponMontage; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETRecoilComponent> RecoilComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponSwayComponent> WeaponSwayComponent;

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

	float LastTimeFired;

	UFUNCTION()
	void OnRep_OwningCharacter(ACharacter* InOldOwner);

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
