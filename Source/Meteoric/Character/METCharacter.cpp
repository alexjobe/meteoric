// Copyright Alex Jobe


#include "METCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Meteoric/Weapon/METWeapon.h"

DEFINE_LOG_CATEGORY(LogMETCharacter);

AMETCharacter::AMETCharacter()
	: bIsAiming(false)
{
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
}

void AMETCharacter::EquipWeapon(AMETWeapon* const InWeapon)
{
	CurrentWeapon = InWeapon;
	if(!ensure(CurrentWeapon)) return;

	CurrentWeapon->OnEquipped(this);
	CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	WeaponEquippedEvent.Broadcast(CurrentWeapon);
}

void AMETCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

void AMETCharacter::AimDownSightsStarted()
{
	if(CurrentWeapon)
	{
		bIsAiming = true;
		AimDownSightsEvent.Broadcast(bIsAiming);
	}
}

void AMETCharacter::AimDownSightsCompleted()
{
	bIsAiming = false;
	AimDownSightsEvent.Broadcast(bIsAiming);
}

void AMETCharacter::FireStarted()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->OnFireStarted();
	}
}

void AMETCharacter::FireHeld()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->OnFireHeld();
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
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMETCharacter::AimDownSightsStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMETCharacter::AimDownSightsCompleted);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMETCharacter::FireStarted);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Ongoing, this, &AMETCharacter::FireHeld);
	}
}