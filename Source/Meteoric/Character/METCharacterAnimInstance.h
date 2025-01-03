// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "METCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMETCharacterAnimInstance();

	//~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//~ End UAnimInstance interface

	UFUNCTION(BlueprintGetter)
	UAnimSequence* GetIdleWeaponAnim() const { return IdleWeaponAnim; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<class AMETCharacter> Character;

	UPROPERTY(Transient)
	TObjectPtr<class AMETWeapon> CurrentWeapon;

	UPROPERTY(Transient)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> IdleWeaponAnim;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Control")
	FRotator ActorControlRotationDelta;
	
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimDownSightsSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Sway")
	FRotator WeaponSwayRotation;

	/* Right Hand Relative to Sight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ADS")
	FTransform RightHandToSight;

	/* Sight Relative to Spine */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform SightToSpine;

	/* Right Hand Relative to Spine */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform RightHandToSpine;

	/* Right Hand Offset from Recoil */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform RecoilOffset;

	UFUNCTION()
	void AnimNotify_EquipWeapon() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Turning", meta=(AllowPrivateAccess = "true"))
	bool bIsTurningInPlace;
	
	void UpdateMovementData();
	void UpdateWeaponSway();
	
	void SetHandRelativeToSight();
	void SetSightRelativeToSpine();
	void SetHandRelativeToSpine();
	void UpdateRecoilOffset();

	UFUNCTION()
	void WeaponManager_OnWeaponEquippedEvent(AMETWeapon* InWeapon);
};
