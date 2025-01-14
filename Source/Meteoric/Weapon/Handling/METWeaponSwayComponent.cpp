// Copyright Alex Jobe


#include "METWeaponSwayComponent.h"

#include "GameFramework/Character.h"

UMETWeaponSwayComponent::UMETWeaponSwayComponent()
	: SwayRateRoll_Default(2.0f)
	, SwayRateYaw_Default(1.0f)
	, SwayRatePitch_Default(1.5f)
	, SwayRateRoll_Aim(0.7f)
	, SwayRateYaw_Aim(0.2f)
	, SwayRatePitch_Aim(0.7f)
	, bIsAiming(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETWeaponSwayComponent::UpdateWeaponSway(const float InDeltaTime)
{
	if(!OwningCharacter) return;
	
	const FRotator ControlRotation = OwningCharacter->GetControlRotation();
	if(PreviousControlRotation.IsSet())
	{
		FRotator Delta = PreviousControlRotation.GetValue() - ControlRotation;
		Delta.Normalize();

		TargetWeaponSway = FMath::InterpSinInOut(TargetWeaponSway, FRotator::ZeroRotator, InDeltaTime * 12.f);

		const float SwayRateRoll  =	bIsAiming ? SwayRateRoll_Aim  : SwayRateRoll_Default;
		const float SwayRateYaw   =	bIsAiming ? SwayRateYaw_Aim   : SwayRateYaw_Default;
		const float SwayRatePitch = bIsAiming ? SwayRatePitch_Aim : SwayRatePitch_Default;

		if(FMath::Abs(Delta.Yaw) > FMath::Abs(Delta.Pitch))
		{
			TargetWeaponSway.Pitch -= Delta.Yaw;
			TargetWeaponSway.Pitch = FMath::Clamp(TargetWeaponSway.Pitch, -SwayRateRoll, SwayRateRoll);

			TargetWeaponSway.Yaw += Delta.Yaw;
			TargetWeaponSway.Yaw = FMath::Clamp(TargetWeaponSway.Yaw , -SwayRateYaw, SwayRateYaw);
		}
		else
		{
			TargetWeaponSway.Roll -= Delta.Pitch;
			TargetWeaponSway.Roll = FMath::Clamp(TargetWeaponSway.Roll , -SwayRatePitch, SwayRatePitch);
		}

		if(CurrentWeaponSway != TargetWeaponSway)
		{
			CurrentWeaponSway = FMath::InterpSinInOut(CurrentWeaponSway, TargetWeaponSway, InDeltaTime * 20.f);
		}

		/*GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("ControlRotationDelta: %s"), *ControlRotation.ToString()));
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString::Printf(TEXT("Weapon Sway: %s"), *WeaponSway.ToString()));
		GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, FString::Printf(TEXT("Delta: %s"), *Delta.ToString()));*/
	}

	PreviousControlRotation = ControlRotation;
}

void UMETWeaponSwayComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	Reset();
	OwningCharacter = InOwningCharacter;
}

void UMETWeaponSwayComponent::OnAimDownSights(bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	CurrentWeaponSway = FRotator::ZeroRotator;
	TargetWeaponSway = FRotator::ZeroRotator;
}

void UMETWeaponSwayComponent::Reset()
{
	OwningCharacter = nullptr;
	PreviousControlRotation = FRotator::ZeroRotator;
	CurrentWeaponSway = FRotator::ZeroRotator;
	TargetWeaponSway = FRotator::ZeroRotator;
	bIsAiming = false;
}