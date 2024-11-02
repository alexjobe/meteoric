// Copyright Alex Jobe


#include "METCharacterAnimInstance.h"

#include "METCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/Weapon/METWeapon.h"

UMETCharacterAnimInstance::UMETCharacterAnimInstance()
	: Character(nullptr)
	, MovementComponent(nullptr)
	, GroundSpeed(0.f)
	, bShouldMove(false)
	, bIsFalling(false)
	, SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, AimAlpha(0.f)
{
}

void UMETCharacterAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AMETCharacter>(TryGetPawnOwner());
	if(!Character) return;
	
	MovementComponent = Character->GetCharacterMovement();
	Character->OnWeaponEquipped().AddUniqueDynamic(this, &UMETCharacterAnimInstance::OnWeaponEquipped);
	Character->OnAimDownSights().AddUniqueDynamic(this, &UMETCharacterAnimInstance::OnAimDownSights);
	
	OnWeaponEquipped(Character->GetCurrentWeapon());
}

void UMETCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(!Character) return;

	UpdateMovementData();
	
	SetActorControlRotationDelta();
	SetSightRelativeToSpine();
	SetHandRelativeToSpine();

	if(Character->IsAiming())
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 1.f, AimDownSightsSpeed * DeltaSeconds);
	}
	else
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 0.f, AimDownSightsSpeed * DeltaSeconds);
	}

	UpdateWeaponSway(DeltaSeconds);
}

void UMETCharacterAnimInstance::UpdateMovementData()
{
	if(!ensure(MovementComponent)) return;
	
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();
	bShouldMove = GroundSpeed > 3.f && !MovementComponent->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.f);
	bIsFalling = MovementComponent->IsFalling();
}

void UMETCharacterAnimInstance::UpdateWeaponSway(float DeltaSeconds)
{
	const FRotator ControlRotation = Character->GetControlRotation();
	if(PreviousControlRotation.IsSet())
	{
		FRotator Delta = PreviousControlRotation.GetValue() - ControlRotation;
		Delta.Normalize();

		TargetWeaponSway = FMath::InterpSinInOut(TargetWeaponSway, FRotator::ZeroRotator, DeltaSeconds * 12.f);

		const float SwayRateRoll = AimAlpha > .8f ? .7f : 3.f;
		const float SwayRateYaw = AimAlpha > .8f ? .2f : 3.f;
		const float SwayRatePitch = AimAlpha > .8f ? .7f : 2.f;

		if(FMath::Abs(Delta.Yaw) > FMath::Abs(Delta.Pitch))
		{
			TargetWeaponSway.Roll -= Delta.Yaw;
			TargetWeaponSway.Roll = FMath::Clamp(TargetWeaponSway.Roll, -SwayRateRoll, SwayRateRoll);

			TargetWeaponSway.Yaw += Delta.Yaw;
			TargetWeaponSway.Yaw = FMath::Clamp(TargetWeaponSway.Yaw , -SwayRateYaw, SwayRateYaw);
		}
		else
		{
			TargetWeaponSway.Pitch -= Delta.Pitch;
			TargetWeaponSway.Pitch = FMath::Clamp(TargetWeaponSway.Pitch , -SwayRatePitch, SwayRatePitch);
		}

		if(WeaponSway != TargetWeaponSway)
		{
			WeaponSway = FMath::InterpSinInOut(WeaponSway, TargetWeaponSway, DeltaSeconds * 20.f);
		}

		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("ControlRotationDelta: %s"), *ControlRotation.ToString()));
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Blue, FString::Printf(TEXT("Weapon Sway: %s"), *WeaponSway.ToString()));
		GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Blue, FString::Printf(TEXT("Delta: %s"), *Delta.ToString()));
	}

	PreviousControlRotation = ControlRotation;
}

void UMETCharacterAnimInstance::SetActorControlRotationDelta()
{
	if(!ensure(Character)) return;
	
	const FRotator ActorRotation = Character->GetActorRotation();
	const FRotator ControlRotation = Character->GetControlRotation();

	FRotator Delta = ActorRotation - ControlRotation;
	Delta.Normalize();

	ActorControlRotationDelta.Pitch = 0.f;
	ActorControlRotationDelta.Yaw = Delta.Yaw;
	ActorControlRotationDelta.Roll = Delta.Pitch / 5.f;
}

void UMETCharacterAnimInstance::SetHandRelativeToSight()
{
	if(!ensure(Character)) return;
	
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const AMETWeapon* Weapon = Character->GetCurrentWeapon();
	const USkeletalMeshComponent* WeaponMesh = Weapon ? Weapon->GetMesh() : nullptr;

	if(CharacterMesh && WeaponMesh)
	{
		const FTransform RightHandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform SightTransform = WeaponMesh->GetSocketTransform(FName("S_Sight"));

		RightHandRelSight = RightHandTransform.GetRelativeTransform(SightTransform);
	}
}

void UMETCharacterAnimInstance::SetSightRelativeToSpine()
{
	if(!ensure(Character)) return;

	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const UCameraComponent* MainCamera = Character->GetMainCamera();

	if (CharacterMesh && MainCamera)
	{
		const FTransform CameraTransform = MainCamera->GetComponentToWorld();
		const FTransform SpineTransform = CharacterMesh->GetBoneTransform(FName("spine_05"));

		SightRelSpine = CameraTransform.GetRelativeTransform(SpineTransform);
		SightRelSpine.SetLocation(SightRelSpine.GetLocation() + SightRelSpine.GetRotation().GetForwardVector() * SightCameraOffset);
	}
	
}

void UMETCharacterAnimInstance::SetHandRelativeToSpine()
{
	RightHandRelSpine = RightHandRelSight * SightRelSpine;
}

void UMETCharacterAnimInstance::OnWeaponEquipped(const AMETWeapon* InWeapon)
{
	if(InWeapon)
	{
		IdleWeaponAnim = InWeapon->GetCharacterIdleWeaponAnim();
		SightCameraOffset = InWeapon->SightCameraOffset;
		AimDownSightsSpeed = InWeapon->AimDownSightsSpeed;
		SetHandRelativeToSight();
	}
}

void UMETCharacterAnimInstance::OnAimDownSights(bool bInIsAiming)
{
	WeaponSway = FRotator::ZeroRotator;
	TargetWeaponSway = FRotator::ZeroRotator;
}
