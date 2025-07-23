// Copyright Alex Jobe


#include "METFootstepComponent.h"


UMETFootstepComponent::UMETFootstepComponent()
	: SlowCycleSpeed(0.f)
	, FastCycleSpeed(0.2f)
	, MaxVerticalAmplitude(2.f)
	, MaxHorizontalAmplitude(1.f)
	, HorizontalPhaseOffset(90.f)
	, MinSpeedForStep(150.f)
	, MaxSpeedForStep(600.f)
	, MinTimeForStep(0.f)
	, MaxTimeForStep(0.5f)
	, InterpToTargetOffsetSpeed(15.f)
	, InterpToZeroSpeed(15.f)
	, FootstepTime(0.f)
	, FootstepOffset(0.f)
	, TargetFootstepOffset(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETFootstepComponent::UpdateFootstep(const float InDeltaTime, const FVector& InPlayerVelocity, bool bInCanStep)
{
	const float PlayerSpeed = InPlayerVelocity.Size();

	if (bInCanStep && PlayerSpeed >= MinSpeedForStep)
	{
		const float FootstepTimeAlpha = FMath::Clamp((FootstepTime - MinTimeForStep) / (MaxTimeForStep - MinTimeForStep), 0.f, 1.f);
		const float SpeedAlpha = FMath::Clamp((PlayerSpeed - MinSpeedForStep) / (MaxSpeedForStep - MinSpeedForStep), 0.f, 1.f);

		const float StepCycleSpeed = FMath::Lerp(SlowCycleSpeed, FastCycleSpeed, SpeedAlpha);
		
		FootstepTime += InDeltaTime * StepCycleSpeed * PlayerSpeed * 0.01f;

		const float VerticalAmplitude = MaxVerticalAmplitude * FootstepTimeAlpha;
		const float HorizontalAmplitude = MaxHorizontalAmplitude * FootstepTimeAlpha;
		
		const float Cycle = FootstepTime * PI * 2;
		TargetFootstepOffset.Z = FMath::Sin(Cycle * 2.f) * VerticalAmplitude;

		const float PhaseOffsetRadians = FMath::DegreesToRadians(HorizontalPhaseOffset);
		TargetFootstepOffset.X = FMath::Sin(Cycle + PhaseOffsetRadians) * HorizontalAmplitude;
	}
	else
	{
		FootstepTime = 0.f;
		TargetFootstepOffset.Z = FMath::InterpSinInOut(static_cast<float>(TargetFootstepOffset.Z), 0.f, InDeltaTime * InterpToZeroSpeed);
		TargetFootstepOffset.X = FMath::InterpSinInOut(static_cast<float>(TargetFootstepOffset.X), 0.f, InDeltaTime * InterpToZeroSpeed);
	}

	if (FootstepOffset != TargetFootstepOffset)
	{
		FootstepOffset = FMath::InterpSinInOut(FootstepOffset, TargetFootstepOffset, InDeltaTime * InterpToTargetOffsetSpeed);
	}

	GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, FString::Printf(TEXT("FootstepOffset: %s"), *FootstepOffset.ToString()));
}
