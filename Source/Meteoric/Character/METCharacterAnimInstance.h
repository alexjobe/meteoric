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

protected:
	UPROPERTY(Transient)
	class AMETCharacter* Character;

	UPROPERTY(Transient)
	class UCharacterMovementComponent* MovementComponent;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	float AimAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform HandRelativeToSight;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform SightRelativeToSpine;

	UPROPERTY(BlueprintReadOnly, Category = "ADS")
	FTransform HandRelativeToSpine;

private:

	void UpdateMovementData();
	void SetControlRotationDelta();
	void SetHandRelativeToSight();
	void SetSightRelativeToSpine();
	void SetHandRelativeToSpine();
	
};
