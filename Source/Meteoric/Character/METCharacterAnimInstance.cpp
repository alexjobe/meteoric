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
	OnWeaponEquipped(Character->GetCurrentWeapon());
}

void UMETCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(!Character) return;

	UpdateMovementData();
	SetControlRotationDelta();
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
		const FTransform RightHandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform LeftHandTransform = CharacterMesh->GetBoneTransform(FName("hand_l"));
		const FTransform SightTransform = WeaponMesh->GetSocketTransform(FName("S_Sight"));

		RightHandRelSight = RightHandTransform.GetRelativeTransform(SightTransform);
		LeftHandRelSight= LeftHandTransform.GetRelativeTransform(SightTransform);
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
	LeftHandRelSpine = LeftHandRelSight * SightRelSpine;
}

void UMETCharacterAnimInstance::OnWeaponEquipped(const AMETWeapon* InWeapon)
{
	if(InWeapon)
	{
		SetHandRelativeToSight();
		SightCameraOffset = InWeapon->SightCameraOffset;
		AimDownSightsSpeed = InWeapon->AimDownSightsSpeed;
	}
}
