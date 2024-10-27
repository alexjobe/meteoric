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
{
}

void UMETCharacterAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AMETCharacter>(TryGetPawnOwner());
	MovementComponent = Character ? Character->GetCharacterMovement() : nullptr;
}

void UMETCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(!Character) return;

	UpdateMovementData();
	SetControlRotationDelta();
	SetHandRelativeToSight();
	SetSightRelativeToSpine();
	SetHandRelativeToSpine();

	if(Character->IsAiming())
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 1.f, Character->AimDownSightsSpeed * DeltaSeconds);
	}
	else
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 0.f, Character->AimDownSightsSpeed * DeltaSeconds);
	}
}

void UMETCharacterAnimInstance::UpdateMovementData()
{
	if(!ensure(MovementComponent)) return;
	
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();
	bShouldMove = GroundSpeed > 3.f && !MovementComponent->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.f);
	bIsFalling = MovementComponent->IsFalling();
}

void UMETCharacterAnimInstance::SetControlRotationDelta()
{
	if(!ensure(Character)) return;
	
	const FRotator ActorRotation = Character->GetActorRotation();
	const FRotator ControlRotation = Character->GetControlRotation();

	FRotator Delta = ActorRotation - ControlRotation;
	Delta.Normalize();

	ControlRotationDelta.Pitch = 0.f;
	ControlRotationDelta.Yaw = Delta.Yaw;
	ControlRotationDelta.Roll = Delta.Pitch / 5.f;
}

void UMETCharacterAnimInstance::SetHandRelativeToSight()
{
	if(!ensure(Character)) return;
	
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const AMETWeapon* Weapon = Character->GetCurrentWeapon();
	const USkeletalMeshComponent* WeaponMesh = Weapon ? Weapon->GetMesh() : nullptr;

	if(CharacterMesh && WeaponMesh)
	{
		const FTransform HandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform SightTransform = WeaponMesh->GetSocketTransform(FName("S_Sight"));

		HandRelativeToSight = HandTransform.GetRelativeTransform(SightTransform);
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

		SightRelativeToSpine = CameraTransform.GetRelativeTransform(SpineTransform);
		SightRelativeToSpine.SetLocation(SightRelativeToSpine.GetLocation() + SightRelativeToSpine.GetRotation().GetForwardVector() * SightCameraOffset);
	}
	
}

void UMETCharacterAnimInstance::SetHandRelativeToSpine()
{
	HandRelativeToSpine = HandRelativeToSight * SightRelativeToSpine;
}
