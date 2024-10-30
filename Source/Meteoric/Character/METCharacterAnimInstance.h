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
	
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintGetter)
	UAnimSequence* GetIdleWeaponAnim() const { return IdleWeaponAnim; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<class AMETCharacter> Character;

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
	FRotator ControlRotationDelta;
	
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimDownSightsSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimAlpha;

	/* Right Hand Relative to Sight */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform RightHandRelSight;

	/* Left Hand Relative to Sight */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform LeftHandRelSight;

	/* Sight Relative to Spine */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform SightRelSpine;

	/* Right Hand Relative to Spine */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform RightHandRelSpine;
	
	/* Left Hand Relative to Spine */
	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform LeftHandRelSpine;

private:

	void UpdateMovementData();
	
	void SetControlRotationDelta();
	void SetHandRelativeToSight();
	void SetSightRelativeToSpine();
	void SetHandRelativeToSpine();

	UFUNCTION()
	void OnWeaponEquipped(const class AMETWeapon* InWeapon);
};
