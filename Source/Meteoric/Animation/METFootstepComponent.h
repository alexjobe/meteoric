// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METFootstepComponent.generated.h"

USTRUCT(BlueprintType)
struct FMETFootstepSettings
{
	GENERATED_BODY()

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	float MaxForwardAmplitude;
	
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
	
	FMETFootstepSettings()
		: SlowCycleSpeed(0.f)
		, FastCycleSpeed(0.2f)
		, MaxVerticalAmplitude(1.f)
		, MaxHorizontalAmplitude(0.5f)
		, MaxForwardAmplitude(0.375f)
		, HorizontalPhaseOffset(90.f)
		, MinSpeedForStep(150.f)
		, MaxSpeedForStep(600.f)
		, MinTimeForStep(0.f)
		, MaxTimeForStep(0.5f)
		, InterpToTargetOffsetSpeed(15.f)
		, InterpToZeroSpeed(15.f)
	{}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METEORIC_API UMETFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	FMETFootstepSettings DefaultSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Footstep")
	FMETFootstepSettings AimSettings;
	
	UMETFootstepComponent();

	void UpdateFootstep(const float InDeltaTime, const FVector& InPlayerVelocity, const bool bInCanStep);

	FVector GetFootstepOffset() const { return FootstepOffset; };

	UFUNCTION()
	void OnAimDownSights(const bool bInIsAiming);

private:
	float FootstepTime;
	FVector FootstepOffset;
	FVector TargetFootstepOffset;
	bool bIsAiming;
};
