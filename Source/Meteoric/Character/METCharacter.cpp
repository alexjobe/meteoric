// Copyright Alex Jobe


#include "METCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogMETCharacter);

AMETCharacter::AMETCharacter()
	: bIsAiming(false)
{
	SetReplicates(true);
	
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -89.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), FName("CameraSocket"));
	CameraBoom->TargetArmLength = 0.f;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom);
	MainCamera->bUsePawnControlRotation = true;

	WeaponManager = CreateDefaultSubobject<UMETWeaponManager>(TEXT("WeaponManager"));
	WeaponManager->OnChangingWeaponsEvent().AddUniqueDynamic(this, &AMETCharacter::OnChangingWeapons);

	InteractionComponent = CreateDefaultSubobject<UMETInteractionComponent>(TEXT("InteractionComponent"));
}

void AMETCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

FRotator AMETCharacter::GetActorControlRotationDelta()
{
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_Authority || LocalRole == ROLE_AutonomousProxy)
	{
		FRotator Delta = GetActorRotation() - GetControlRotation();
		Delta.Normalize();

		ActorControlRotationDelta.Pitch = 0.f;
		ActorControlRotationDelta.Yaw = Delta.Yaw;

		// Divide by five spine bones
		ActorControlRotationDelta.Roll = Delta.Pitch / 5.f;
	}
	
	return ActorControlRotationDelta;
}

void AMETCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMETCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * .4f);
		AddControllerPitchInput(LookAxisVector.Y * .4f);
	}
}

void AMETCharacter::AimingStarted()
{
	if(!ensure(WeaponManager) || WeaponManager->IsChangingWeapons()) return;
	
	SetAiming(true);

	if(!HasAuthority())
	{
		Server_SetAiming(true);
	}
}

void AMETCharacter::AimingCompleted()
{
	SetAiming(false);
	
	if(!HasAuthority())
	{
		Server_SetAiming(false);
	}
}

void AMETCharacter::FireActionStarted()
{
	if(!ensure(WeaponManager)) return;
	AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon();
	if(!CurrentWeapon || !CurrentWeapon->CanFire() || WeaponManager->IsChangingWeapons()) return;
	
	Fire(false);

	if(!HasAuthority())
	{
		Server_Fire(false);
	}
}

void AMETCharacter::FireActionHeld()
{
	if(!ensure(WeaponManager)) return;
	AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon();
	if(!CurrentWeapon || !CurrentWeapon->CanFire() || WeaponManager->IsChangingWeapons()) return;
	
	Fire(true);
	
	if(!HasAuthority())
	{
		Server_Fire(true);
	}
}

void AMETCharacter::Fire(bool bInHeld)
{
	if(AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon())
	{
		CurrentWeapon->Fire(bInHeld);
	}

	if(HasAuthority())
	{
		Multicast_Fire(bInHeld);
	}
}

void AMETCharacter::Server_Fire_Implementation(bool bInHeld)
{
	Fire(bInHeld);
}

void AMETCharacter::Multicast_Fire_Implementation(bool bInHeld)
{
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_SimulatedProxy)
	{
		Fire(bInHeld);
	}
}

void AMETCharacter::SetAiming(bool bInIsAiming)
{
	if(const AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon())
	{
		bIsAiming = bInIsAiming;
		CurrentWeapon->OnAimDownSights(bIsAiming);
		AimDownSightsEvent.Broadcast(bIsAiming);
	}
}

void AMETCharacter::Server_SetAiming_Implementation(bool bInIsAiming)
{
	SetAiming(bInIsAiming);
}

void AMETCharacter::OnRep_IsAiming()
{
	SetAiming(bIsAiming);
}

void AMETCharacter::OnChangingWeapons(bool bInIsChangingWeapons)
{
	if(bInIsChangingWeapons && bIsAiming)
	{
		AimingCompleted();
	}
}

void AMETCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMETCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMETCharacter::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMETCharacter::AimingStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMETCharacter::AimingCompleted);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMETCharacter::FireActionStarted);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Ongoing, this, &AMETCharacter::FireActionHeld);
	}

	if(WeaponManager) WeaponManager->SetupPlayerInputComponent(PlayerInputComponent);
	if(InteractionComponent) InteractionComponent->SetupPlayerInputComponent(PlayerInputComponent);
}

void AMETCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMETCharacter, ActorControlRotationDelta, COND_SkipOwner)
	DOREPLIFETIME_CONDITION(AMETCharacter, bIsAiming, COND_SkipOwner)
}