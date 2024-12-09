// Copyright Alex Jobe


#include "METPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
#include "Meteoric/Player/METPlayerState.h"
#include "Meteoric/UI/HUD/METHUD.h"
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
	ResetControlRotation();
}

void AMETPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
	ResetControlRotation();
}

void AMETPlayerCharacter::InitAbilityActorInfo()
{
	AMETPlayerState* PS = GetPlayerState<AMETPlayerState>();
	check(PS)
	
	if (UMETAbilitySystemComponent* ASC = Cast<UMETAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
	{
		AbilitySystemComponent = ASC;
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AbilitySystemComponent->SetTagMapCount(METGameplayTags::State_Dead, 0);
	}

	InitializeDefaultAttributes();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->IsLocalController())
	{
		if (AMETHUD* MetHUD = Cast<AMETHUD>(PlayerController->GetHUD()))
		{
			MetHUD->Initialize(AbilitySystemComponent);
		}
	}
}

void AMETPlayerCharacter::ResetControlRotation() const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->IsLocalController())
	{
		PlayerController->SetControlRotation(FRotator(0.f, 180.f, 0.f));
	}
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
	}

	if(WeaponManager) WeaponManager->SetupPlayerInputComponent(PlayerInputComponent);
	if(InteractionComponent) InteractionComponent->SetupPlayerInputComponent(PlayerInputComponent);
}
