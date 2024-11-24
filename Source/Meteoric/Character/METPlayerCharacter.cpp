// Copyright Alex Jobe


#include "METPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
#include "Meteoric/Player/METPlayerState.h"
#include "Meteoric/Weapon/METWeaponManager.h"

AMETPlayerCharacter::AMETPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), FName("CameraSocket"));
	CameraBoom->TargetArmLength = 0.f;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	MainCamera->SetupAttachment(CameraBoom);
	MainCamera->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<UMETInteractionComponent>(TEXT("InteractionComponent"));
}

void AMETPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
}

void AMETPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
}

void AMETPlayerCharacter::InitAbilityActorInfo()
{
	AMETPlayerState* PS = GetPlayerState<AMETPlayerState>();
	check(PS)
	
	if (UMETAbilitySystemComponent* ASC = Cast<UMETAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
	{
		AbilitySystemComponent = ASC;
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
	}

	InitializeDefaultAttributes();
}

void AMETPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMETPlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMETPlayerCharacter::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AMETPlayerCharacter::AimingStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AMETPlayerCharacter::AimingCompleted);

		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMETPlayerCharacter::FireActionStarted);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Ongoing, this, &AMETPlayerCharacter::FireActionHeld);
	}

	if(WeaponManager) WeaponManager->SetupPlayerInputComponent(PlayerInputComponent);
	if(InteractionComponent) InteractionComponent->SetupPlayerInputComponent(PlayerInputComponent);
}
