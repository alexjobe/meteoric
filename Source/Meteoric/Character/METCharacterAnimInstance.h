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
	UAnimSequence* GetCurrentIdleAnim() const { return CurrentIdleAnim; }

	UFUNCTION(BlueprintGetter)
	UAnimSequence* GetPreviousIdleAnim() const { return PreviousIdleAnim; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<class AMETCharacter> Character;

	UPROPERTY(Transient)
	TObjectPtr<class AMETWeapon> CurrentWeapon;

	UPROPERTY(Transient)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UMETFootstepComponent> FootstepComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> CurrentIdleAnim;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	TObjectPtr<UAnimSequence> PreviousIdleAnim;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float IdleBlendAlpha;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float IdleBlendDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsFalling;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouched;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float CrouchBlendTime;

	UPROPERTY(BlueprintReadOnly, Category = "Control")
	FRotator ActorControlRotationDelta;
	
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimDownSightsSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Sway")
	FTransform WeaponSway;

	UPROPERTY(BlueprintReadOnly, Category = "Footstep")
	FVector FootstepOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ADS")
	FName CameraRootBone;

	/* Right Hand Relative to Sight */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ADS")
	FTransform RightHandToSight;

	/* Sight Relative to Camera Root */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform SightToCameraRoot;

	/* Right Hand Relative to Camera Root */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform RightHandToCameraRoot;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	float LeftHandAttachAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	FTransform LeftHandToRightHand;
	
	/* Right Hand Offset from Recoil */
	UPROPERTY(BlueprintReadOnly, Category = "Recoil")
	FTransform RecoilOffset;

	UFUNCTION()
	void AnimNotify_EquipWeapon() const;

private:
	UPROPERTY(BlueprintReadOnly, Category = "Turning", meta=(AllowPrivateAccess = "true"))
	bool bIsTurningInPlace;
	
	void UpdateMovementData();
	void UpdateWeaponSway();
	
	void SetRightHandToSight();
	void SetSightToCameraRoot();
	void SetRightHandToCameraRoot();
	void SetLeftHandToRightHand();
	void UpdateRecoilOffset();

	UFUNCTION()
	void WeaponManager_OnWeaponEquippedEvent(AMETWeapon* InWeapon);
};
