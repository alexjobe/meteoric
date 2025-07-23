// Copyright Alex Jobe


#include "METPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PMCoverUserComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/Animation/METFootstepComponent.h"
#include "Meteoric/GameMode/METGameModeBase.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
#include "Meteoric/Player/METPlayerState.h"
#include "Meteoric/UI/HUD/METHUD.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"
#include "Navigation/CrowdManager.h"


static TAutoConsoleVariable<int32> CVarInvinciblePlayer(
	TEXT("METPlayerCharacter.Invincible"),
	0,
	TEXT("If 1, player won't die")
);

AMETPlayerCharacter::AMETPlayerCharacter()
	: AimTraceRange(3000.f)
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	check(CameraBoom);
	CameraBoom->SetupAttachment(GetMesh(), FName("S_Camera"));
	CameraBoom->TargetArmLength = 0.f;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	check(MainCamera);
	MainCamera->SetupAttachment(CameraBoom);
	MainCamera->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<UMETInteractionComponent>(TEXT("InteractionComponent"));
	FootstepComponent = CreateDefaultSubobject<UMETFootstepComponent>(TEXT("FootstepComponent"));

	if (ensure(CoverUserComponent))
	{
		CoverUserComponent->bShouldHoldSpot = false;
	}

	CharacterConfig.AimInterpSpeed = 40.f;
}

void AMETPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if(UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this); ensure(CrowdManager))
		{
			CrowdManager->RegisterAgent(this);
		}
	}
}

void AMETPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FootstepComponent)
	{
		const bool bCanStep = !GetCharacterMovement()->IsFalling() && !IsDead();
		FootstepComponent->UpdateFootstep(DeltaTime, GetVelocity(), bCanStep);
	}
}

void AMETPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitAbilityActorInfo();
	ResetControlRotation();
	if (ensure(AmmoManager))
	{
		AmmoManager->OnPossessed();
	}
}

void AMETPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitAbilityActorInfo();
	AddCharacterAbilities();
	ResetControlRotation();
	if (ensure(AmmoManager))
	{
		AmmoManager->OnPossessed();
	}
}

FVector AMETPlayerCharacter::GetFocalPoint() const
{
	if (ensure(MainCamera))
	{
		FMinimalViewInfo ViewInfo;
		MainCamera->GetCameraView(GetWorld()->DeltaTimeSeconds, ViewInfo);
		return ViewInfo.Location + ViewInfo.Rotation.Quaternion().GetForwardVector() * AimTraceRange;
	}
	return GetActorLocation();
}

FTransform AMETPlayerCharacter::GetEyesPosition() const
{
	if (ensure(MainCamera))
	{
		return MainCamera->GetComponentToWorld();
	}
	return Super::GetEyesPosition();
}

FVector AMETPlayerCharacter::GetCrowdAgentLocation() const
{
	return GetActorLocation();
}

FVector AMETPlayerCharacter::GetCrowdAgentVelocity() const
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement(); ensure(MovementComponent))
	{
		return MovementComponent->GetVelocityForRVOConsideration();
	}
	return FVector::ZeroVector;
}

void AMETPlayerCharacter::GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const
{
	if (const UCapsuleComponent* Capsule = GetCapsuleComponent(); ensure(Capsule))
	{
		CylinderRadius = Capsule->GetScaledCapsuleRadius();
		CylinderHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	}
}

float AMETPlayerCharacter::GetCrowdAgentMaxSpeed() const
{
	if (const UCharacterMovementComponent* MovementComponent = GetCharacterMovement(); ensure(MovementComponent))
	{
		return MovementComponent->GetMaxSpeed();
	}
	return 0.f;
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
		AttributeSet = PS->GetAttributeSet();
	}

	InitializeDefaultAttributes();
	BindAttributeChangedCallbacks();

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->IsLocalController())
	{
		if (AMETHUD* MetHUD = Cast<AMETHUD>(PlayerController->GetHUD()))
		{
			MetHUD->Initialize(AbilitySystemComponent);
		}
	}
}

void AMETPlayerCharacter::Die()
{
#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarInvinciblePlayer.GetValueOnAnyThread() == 1)
	{
		// Invincible for debugging
		return;
	}
#endif
	
	Super::Die();
	
	if (HasAuthority())
	{
		if (AMETGameModeBase* GameMode = Cast<AMETGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->PlayerDied(GetController());
		}

		UnregisterCrowdAgent();
	}
}

void AMETPlayerCharacter::ResetControlRotation() const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController && PlayerController->IsLocalController())
	{
		PlayerController->SetControlRotation(GetActorRotation());
	}
}

void AMETPlayerCharacter::UnregisterCrowdAgent()
{
	if (HasAuthority())
	{
		if(UCrowdManager* CrowdManager = UCrowdManager::GetCurrent(this))
		{
			CrowdManager->UnregisterAgent(this);
		}
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

inline void AMETPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnregisterCrowdAgent();
}
