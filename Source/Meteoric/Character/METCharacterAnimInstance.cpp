// Copyright Alex Jobe


#include "METCharacterAnimInstance.h"

#include "METCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/Animation/METFootstepComponent.h"
#include "Meteoric/Weapon/Handling/METRecoilComponent.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/Handling/METWeaponSwayComponent.h"

UMETCharacterAnimInstance::UMETCharacterAnimInstance()
	: IdleBlendAlpha(1.f)
	, IdleBlendDuration(0.25f)
	, Velocity(0.f)
	, Direction(0.f)
	, GroundSpeed(0.f)
	, bShouldMove(false)
	, bIsFalling(false)
	, bIsCrouched(false)
	, CrouchBlendTime(0.2f)
	, ActorControlRotationDelta(0.f)
	, SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, AimAlpha(0.f)
	, FootstepOffset(0.f)
	, LeftHandAttachAlpha(0.f)
	, bIsTurningInPlace(false)
{
}

void UMETCharacterAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AMETCharacter>(TryGetPawnOwner());
	if(!Character) return;
	
	MovementComponent = Character->GetCharacterMovement();
	FootstepComponent = Character->FindComponentByClass<UMETFootstepComponent>();

	if(UMETWeaponManager* WeaponManager = Character->GetWeaponManager())
	{
		WeaponManager->OnWeaponEquippedEvent().AddUniqueDynamic(this, &UMETCharacterAnimInstance::WeaponManager_OnWeaponEquippedEvent);
		WeaponManager_OnWeaponEquippedEvent(WeaponManager->GetCurrentWeapon());
	}
}

void UMETCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(!Character) return;

	UpdateMovementData();
	
	ActorControlRotationDelta = Character->GetActorAimRotationDelta();
	bIsTurningInPlace = Character->IsTurningInPlace();
	bIsCrouched = Character->bIsCrouched;
	
	SetSightToCameraRoot();
	SetRightHandToCameraRoot();
	SetLeftHandToRightHand();
	UpdateRecoilOffset();

	if (IdleBlendAlpha < 1.f && IdleBlendDuration > 0.f)
	{
		IdleBlendAlpha = FMath::Clamp(IdleBlendAlpha + DeltaSeconds / IdleBlendDuration, 0.f, 1.f);
	}

	if(Character->IsAiming())
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 1.f, AimDownSightsSpeed * DeltaSeconds);
	}
	else
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 0.f, AimDownSightsSpeed * DeltaSeconds);
	}

	if (Character->ShouldEnableLeftHandIK())
	{
		LeftHandAttachAlpha = FMath::InterpSinInOut(LeftHandAttachAlpha, 1.f, AimDownSightsSpeed * DeltaSeconds);
	}
	else
	{
		LeftHandAttachAlpha = FMath::InterpSinInOut(LeftHandAttachAlpha, 0.f, AimDownSightsSpeed * DeltaSeconds);
	}

	if (FootstepComponent)
	{
		FootstepOffset = FootstepComponent->GetFootstepOffset();
	}

	UpdateWeaponSway();
}

void UMETCharacterAnimInstance::AnimNotify_EquipWeapon() const
{
	if(UMETWeaponManager* WeaponManager = Character->GetWeaponManager())
	{
		WeaponManager->OnEquipWeaponNotify();
	}
}

void UMETCharacterAnimInstance::UpdateMovementData()
{
	if(!ensure(Character) || !ensure(MovementComponent)) return;
	
	Velocity = MovementComponent->Velocity;
	GroundSpeed = Velocity.Size2D();

	const FVector LocalVelocity = Character->GetActorRotation().UnrotateVector(Velocity);
	Direction = LocalVelocity.Rotation().Yaw;
	
	bShouldMove = Character->IsMoving();
	bIsFalling = MovementComponent->IsFalling();
}

void UMETCharacterAnimInstance::UpdateWeaponSway()
{
	if(CurrentWeapon)
	{
		WeaponSway = CurrentWeapon->GetWeaponSwayComponent()->GetWeaponSway();
	}
	else
	{
		WeaponSway = FTransform::Identity;
	}
}

void UMETCharacterAnimInstance::SetRightHandToSight()
{
	if(!ensure(Character)) return;
	
	/* If we already precomputed the value, return */
	if(!RightHandToSight.Equals(FTransform::Identity)) return;
	
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const USkeletalMeshComponent* WeaponMesh = CurrentWeapon ? CurrentWeapon->GetMesh() : nullptr;

	if(CharacterMesh && WeaponMesh)
	{
		const FTransform RightHandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform SightTransform = WeaponMesh->GetSocketTransform(FName("S_Sight"));

		RightHandToSight = RightHandTransform.GetRelativeTransform(SightTransform);
		CurrentWeapon->RightHandToSight = RightHandToSight;
	}
}

void UMETCharacterAnimInstance::SetSightToCameraRoot()
{
	if(!ensure(Character)) return;

	if (const USkeletalMeshComponent* CharacterMesh = Character->GetMesh(); ensure(CharacterMesh))
	{
		const FTransform CameraTransform = Character->GetEyesPosition();
		const FTransform CameraRootBoneTransform = CharacterMesh->GetBoneTransform(CameraRootBone);

		SightToCameraRoot = CameraTransform.GetRelativeTransform(CameraRootBoneTransform);
		SightToCameraRoot.SetLocation(SightToCameraRoot.GetLocation() + SightToCameraRoot.GetRotation().GetForwardVector() * SightCameraOffset);
	}
	
}

void UMETCharacterAnimInstance::SetRightHandToCameraRoot()
{
	RightHandToCameraRoot = RightHandToSight * SightToCameraRoot;
}

void UMETCharacterAnimInstance::SetLeftHandToRightHand()
{
	if(!ensure(Character)) return;
	
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const USkeletalMeshComponent* WeaponMesh = CurrentWeapon ? CurrentWeapon->GetMesh() : nullptr;

	if(CharacterMesh && WeaponMesh)
	{
		const FTransform RightHandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform LeftHandAttachTransform = WeaponMesh->GetSocketTransform(FName("S_LeftHandAttach"));
		LeftHandToRightHand = LeftHandAttachTransform.GetRelativeTransform(RightHandTransform);
	}
}

void UMETCharacterAnimInstance::UpdateRecoilOffset()
{
	if(CurrentWeapon)
	{
		RecoilOffset = CurrentWeapon->GetRecoilComponent()->GetSpringRecoilTransform();
	}
}

void UMETCharacterAnimInstance::WeaponManager_OnWeaponEquippedEvent(AMETWeapon* InWeapon)
{
	// Equipped weapon can be null
	CurrentWeapon = InWeapon;
	
	RecoilOffset = FTransform::Identity;
	WeaponSway = FTransform::Identity;
	RightHandToSight = FTransform::Identity;
	SightToCameraRoot = FTransform::Identity;
	RightHandToCameraRoot = FTransform::Identity;
	LeftHandToRightHand = FTransform::Identity;
	
	if(CurrentWeapon)
	{
		PreviousIdleAnim = CurrentIdleAnim;
		CurrentIdleAnim = CurrentWeapon->GetCharacterIdleWeaponAnim();
		IdleBlendAlpha = PreviousIdleAnim ? 0.f : 1.f;
		SightCameraOffset = CurrentWeapon->SightCameraOffset;
		AimDownSightsSpeed = CurrentWeapon->AimDownSightsSpeed;
		RightHandToSight = CurrentWeapon->RightHandToSight;
		SetRightHandToSight();
	}
}
