// Copyright Alex Jobe


#include "METWeaponSwayComponent.h"

#include "GameFramework/Character.h"

UMETWeaponSwayComponent::UMETWeaponSwayComponent()
	: RotationSwaySpeed(20.f)
	, PositionSwaySpeed(20.f)
	, RotationInterpToZeroSpeed(12.f)
	, PositionInterpToZeroSpeed(12.f)
	, bIsAiming(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	
	DefaultSwaySettings.MaxSwayRoll = 2.0f;
	DefaultSwaySettings.MaxSwayYaw = 1.0f;
	DefaultSwaySettings.MaxSwayPitch = 1.5f;
	
	AimSwaySettings.MaxSwayPitch = 0.7f;
	AimSwaySettings.MaxSwayLateral = 0.15f;
}

void UMETWeaponSwayComponent::UpdateWeaponSway(const float InDeltaTime)
{
	if(!OwningCharacter) return;
	
	const FRotator ControlRotation = OwningCharacter->GetControlRotation();
	if(PreviousControlRotation.IsSet())
	{
		FRotator ControlRotationDelta = PreviousControlRotation.GetValue() - ControlRotation;
		ControlRotationDelta.Normalize();

		FRotator CurrentWeaponSwayRotation = CurrentWeaponSway.Rotator();
		FRotator TargetWeaponSwayRotation  = TargetWeaponSway.Rotator();
		FVector CurrentWeaponSwayPosition  = CurrentWeaponSway.GetLocation();
		FVector TargetWeaponSwayPosition   = TargetWeaponSway.GetLocation();
		
		TargetWeaponSwayRotation = FMath::InterpSinInOut(TargetWeaponSwayRotation, FRotator::ZeroRotator, InDeltaTime * RotationInterpToZeroSpeed);
		TargetWeaponSwayPosition = FMath::InterpSinInOut(TargetWeaponSwayPosition, FVector::ZeroVector, InDeltaTime * PositionInterpToZeroSpeed);

		const float MaxSwayRoll     = bIsAiming ? AimSwaySettings.MaxSwayRoll     : DefaultSwaySettings.MaxSwayRoll;
		const float MaxSwayYaw      = bIsAiming ? AimSwaySettings.MaxSwayYaw      : DefaultSwaySettings.MaxSwayYaw;
		const float MaxSwayPitch    = bIsAiming ? AimSwaySettings.MaxSwayPitch    : DefaultSwaySettings.MaxSwayPitch;
		const float MaxSwayLateral	= bIsAiming ? AimSwaySettings.MaxSwayLateral  : DefaultSwaySettings.MaxSwayLateral;
		const float MaxSwayVertical = bIsAiming ? AimSwaySettings.MaxSwayVertical : DefaultSwaySettings.MaxSwayVertical;

		if(FMath::Abs(ControlRotationDelta.Yaw) > FMath::Abs(ControlRotationDelta.Pitch))
		{
			TargetWeaponSwayRotation.Pitch -= ControlRotationDelta.Yaw;
			TargetWeaponSwayRotation.Pitch = FMath::Clamp(TargetWeaponSwayRotation.Pitch, -MaxSwayRoll, MaxSwayRoll);

			TargetWeaponSwayRotation.Yaw += ControlRotationDelta.Yaw;
			TargetWeaponSwayRotation.Yaw = FMath::Clamp(TargetWeaponSwayRotation.Yaw , -MaxSwayYaw, MaxSwayYaw);

			TargetWeaponSwayPosition.X -= ControlRotationDelta.Yaw;
			TargetWeaponSwayPosition.X = FMath::Clamp(TargetWeaponSwayPosition.X, -MaxSwayLateral, MaxSwayLateral);
		}
		else
		{
			TargetWeaponSwayRotation.Roll -= ControlRotationDelta.Pitch;
			TargetWeaponSwayRotation.Roll = FMath::Clamp(TargetWeaponSwayRotation.Roll , -MaxSwayPitch, MaxSwayPitch);

			TargetWeaponSwayPosition.Z -= ControlRotationDelta.Pitch;
			TargetWeaponSwayPosition.Z = FMath::Clamp(TargetWeaponSwayPosition.Z, -MaxSwayVertical, MaxSwayVertical);
		}

		if(CurrentWeaponSwayRotation != TargetWeaponSwayRotation)
		{
			CurrentWeaponSwayRotation = FMath::InterpSinInOut(CurrentWeaponSwayRotation, TargetWeaponSwayRotation, InDeltaTime * RotationSwaySpeed);
		}

		if (CurrentWeaponSwayPosition != TargetWeaponSwayPosition)
		{
			CurrentWeaponSwayPosition = FMath::InterpSinInOut(CurrentWeaponSwayPosition, TargetWeaponSwayPosition, InDeltaTime * PositionSwaySpeed);
		}

		CurrentWeaponSway.SetRotation(CurrentWeaponSwayRotation.Quaternion());
		TargetWeaponSway.SetRotation(TargetWeaponSwayRotation.Quaternion());
		CurrentWeaponSway.SetLocation(CurrentWeaponSwayPosition);
		TargetWeaponSway.SetLocation(TargetWeaponSwayPosition);

		/*GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("ControlRotationDelta: %s"), *ControlRotation.ToString()));
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString::Printf(TEXT("Current Weapon Sway: %s"), *CurrentWeaponSway.ToString()));
		GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, FString::Printf(TEXT("Delta: %s"), *ControlRotationDelta.ToString()));*/
	}

	PreviousControlRotation = ControlRotation;
}

void UMETWeaponSwayComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	Reset();
	OwningCharacter = InOwningCharacter;
}

void UMETWeaponSwayComponent::OnAimDownSights(const bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	CurrentWeaponSway = FTransform::Identity;
	TargetWeaponSway = FTransform::Identity;
}

void UMETWeaponSwayComponent::Reset()
{
	OwningCharacter = nullptr;
	PreviousControlRotation = FRotator::ZeroRotator;
	CurrentWeaponSway = FTransform::Identity;
	TargetWeaponSway = FTransform::Identity;
	bIsAiming = false;
}