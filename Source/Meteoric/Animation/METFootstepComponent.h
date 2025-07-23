// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METFootstepComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METEORIC_API UMETFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Step frequency at minimum speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float SlowCycleSpeed;

	// Step frequency at maximum speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float FastCycleSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MaxVerticalAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MaxHorizontalAmplitude;

	/* Horizontal sway offset in degrees. Determines where to start on the sine wave. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep", meta=(ClampMin="0", ClampMax="360"))
	float HorizontalPhaseOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MinSpeedForStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MaxSpeedForStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MinTimeForStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MaxTimeForStep;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float InterpToTargetOffsetSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float InterpToZeroSpeed;
	
	UMETFootstepComponent();

	void UpdateFootstep(const float InDeltaTime, const FVector& InPlayerVelocity, bool bInCanStep);

	FVector GetFootstepOffset() const { return FootstepOffset; };

private:
	float FootstepTime;
	FVector FootstepOffset;
	FVector TargetFootstepOffset;
};
