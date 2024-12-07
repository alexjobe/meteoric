// Copyright Alex Jobe


#include "METCharacterAnimInstance.h"

#include "METCharacter.h"
#include "METPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/Weapon/METRecoilComponent.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/METWeaponSwayComponent.h"

UMETCharacterAnimInstance::UMETCharacterAnimInstance()
	: GroundSpeed(0.f)
	, bShouldMove(false)
	, bIsFalling(false)
	, SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, AimAlpha(0.f)
	, bIsTurningInPlace(false)
{
}

void UMETCharacterAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<AMETCharacter>(TryGetPawnOwner());
	if(!Character) return;
	
	MovementComponent = Character->GetCharacterMovement();

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
	
	ActorControlRotationDelta = Character->GetActorControlRotationDelta();
	bIsTurningInPlace = Character->IsTurningInPlace();
	
	SetSightRelativeToSpine();
	SetHandRelativeToSpine();
	UpdateRecoilOffset();

	if(Character->IsAiming())
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 1.f, AimDownSightsSpeed * DeltaSeconds);
	}
	else
	{
		AimAlpha = FMath::InterpSinInOut(AimAlpha, 0.f, AimDownSightsSpeed * DeltaSeconds);
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
	bShouldMove = Character->IsMoving();
	bIsFalling = MovementComponent->IsFalling();
}

void UMETCharacterAnimInstance::UpdateWeaponSway()
{
	if(CurrentWeapon)
	{
		WeaponSwayRotation = CurrentWeapon->GetWeaponSwayComponent()->GetWeaponSway();
	}
	else
	{
		WeaponSwayRotation = FRotator::ZeroRotator;
	}
}

void UMETCharacterAnimInstance::SetHandRelativeToSight()
{
	if(!ensure(Character)) return;
	
	/* If we already precomputed the value, return */
	if(!RightHandToSight.Equals(FTransform::Identity)) return;
	
	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const UMETWeaponManager* WeaponManager = Character->GetWeaponManager();
	AMETWeapon* Weapon = WeaponManager ? WeaponManager->GetCurrentWeapon() : nullptr;
	const USkeletalMeshComponent* WeaponMesh = Weapon ? Weapon->GetMesh() : nullptr;

	if(CharacterMesh && WeaponMesh)
	{
		const FTransform RightHandTransform = CharacterMesh->GetBoneTransform(FName("hand_r"));
		const FTransform SightTransform = WeaponMesh->GetSocketTransform(FName("S_Sight"));

		RightHandToSight = RightHandTransform.GetRelativeTransform(SightTransform);
		Weapon->RightHandToSight = RightHandToSight;
	}
}

void UMETCharacterAnimInstance::SetSightRelativeToSpine()
{
	if(!ensure(Character)) return;

	const USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	const AMETPlayerCharacter* PlayerCharacter = Cast<AMETPlayerCharacter>(Character);
	const UCameraComponent* MainCamera = PlayerCharacter ? PlayerCharacter->GetMainCamera() : nullptr;

	if (CharacterMesh && MainCamera)
	{
		const FTransform CameraTransform = MainCamera->GetComponentToWorld();
		const FTransform SpineTransform = CharacterMesh->GetBoneTransform(FName("spine_05"));

		SightToSpine = CameraTransform.GetRelativeTransform(SpineTransform);
		SightToSpine.SetLocation(SightToSpine.GetLocation() + SightToSpine.GetRotation().GetForwardVector() * SightCameraOffset);
	}
	
}

void UMETCharacterAnimInstance::SetHandRelativeToSpine()
{
	RightHandToSpine = RightHandToSight * SightToSpine;
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
	WeaponSwayRotation = FRotator::ZeroRotator;
	RightHandToSight = FTransform::Identity;
	SightToSpine = FTransform::Identity;
	RightHandToSpine = FTransform::Identity;
	
	if(CurrentWeapon)
	{
		IdleWeaponAnim = CurrentWeapon->GetCharacterIdleWeaponAnim();
		SightCameraOffset = CurrentWeapon->SightCameraOffset;
		AimDownSightsSpeed = CurrentWeapon->AimDownSightsSpeed;
		RightHandToSight = CurrentWeapon->RightHandToSight;
		SetHandRelativeToSight();
	}
}