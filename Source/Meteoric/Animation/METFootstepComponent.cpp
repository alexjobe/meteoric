// Copyright Alex Jobe


#include "METFootstepComponent.h"


UMETFootstepComponent::UMETFootstepComponent()
	: FootstepTime(0.f)
	, FootstepOffset(0.f)
	, TargetFootstepOffset(0.f)
	, bIsAiming(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	AimSettings.MaxVerticalAmplitude = 0.2f;
	AimSettings.MaxHorizontalAmplitude = 0.1f;
	AimSettings.MaxForwardAmplitude = 0.075f;
}

void UMETFootstepComponent::UpdateFootstep(const float InDeltaTime, const FVector& InPlayerVelocity, const bool bInCanStep)
{
	const FMETFootstepSettings& Settings = bIsAiming ? AimSettings : DefaultSettings;
	const float PlayerSpeed = InPlayerVelocity.Size();
	const float HorizontalDirectionModifier = InPlayerVelocity.Y < 0.f ? -1.f : 1.f;
	const float ForwardDirectionModifier = InPlayerVelocity.X < 0.f ? -1.f : 1.f;

	if (bInCanStep && PlayerSpeed >= Settings.MinSpeedForStep)
	{
		const float FootstepTimeAlpha = FMath::Clamp((FootstepTime - Settings.MinTimeForStep) / (Settings.MaxTimeForStep - Settings.MinTimeForStep), 0.f, 1.f);
		const float SpeedAlpha = FMath::Clamp((PlayerSpeed - Settings.MinSpeedForStep) / (Settings.MaxSpeedForStep - Settings.MinSpeedForStep), 0.f, 1.f);

		const float StepCycleSpeed = FMath::Lerp(Settings.SlowCycleSpeed, Settings.FastCycleSpeed, SpeedAlpha);
		
		FootstepTime += InDeltaTime * StepCycleSpeed * PlayerSpeed * 0.01f; // Scale step cycle by player speed

		const float VerticalAmplitude = Settings.MaxVerticalAmplitude * FootstepTimeAlpha;
		const float HorizontalAmplitude = Settings.MaxHorizontalAmplitude * FootstepTimeAlpha;
		const float ForwardAmplitude = Settings.MaxForwardAmplitude * FootstepTimeAlpha;
		
		const float Cycle = FootstepTime * PI * 2;
		TargetFootstepOffset.Z = FMath::Sin(Cycle * 2.f) * VerticalAmplitude;

		const float PhaseOffsetRadians = FMath::DegreesToRadians(Settings.HorizontalPhaseOffset);
		TargetFootstepOffset.X = FMath::Sin(Cycle + PhaseOffsetRadians) * HorizontalAmplitude * HorizontalDirectionModifier;
		
		TargetFootstepOffset.Y = FMath::Cos(Cycle + PhaseOffsetRadians) * ForwardAmplitude * ForwardDirectionModifier;
	}
	else
	{
		FootstepTime = 0.f;
		TargetFootstepOffset.Z = FMath::InterpSinInOut(static_cast<float>(TargetFootstepOffset.Z), 0.f, InDeltaTime * Settings.InterpToZeroSpeed);
		TargetFootstepOffset.X = FMath::InterpSinInOut(static_cast<float>(TargetFootstepOffset.X), 0.f, InDeltaTime * Settings.InterpToZeroSpeed);
		TargetFootstepOffset.Y = FMath::InterpSinInOut(static_cast<float>(TargetFootstepOffset.Y), 0.f, InDeltaTime * Settings.InterpToZeroSpeed);
	}

	if (FootstepOffset != TargetFootstepOffset)
	{
		FootstepOffset = FMath::InterpSinInOut(FootstepOffset, TargetFootstepOffset, InDeltaTime * Settings.InterpToTargetOffsetSpeed);
	}
	
	//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, FString::Printf(TEXT("FootstepOffset: %s"), *FootstepOffset.ToString()));
	//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Blue, FString::Printf(TEXT("TargetFootstepOffset: %s"), *TargetFootstepOffset.ToString()));
	//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Blue, FString::Printf(TEXT("Velocity: %s"), *InPlayerVelocity.ToString()));
}

void UMETFootstepComponent::OnAimDownSights(const bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
}
