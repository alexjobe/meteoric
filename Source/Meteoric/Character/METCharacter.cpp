// Copyright Alex Jobe


#include "METCharacter.h"

#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogMETCharacter);

AMETCharacter::AMETCharacter()
	: bIsAiming(false)
	, bIsTurningInPlace(false)
{
	SetReplicates(true);
	
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -89.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	GetMesh()->AddTickPrerequisiteActor(this);

	WeaponManager = CreateDefaultSubobject<UMETWeaponManager>(TEXT("WeaponManager"));
	WeaponManager->OnChangingWeaponsEvent().AddUniqueDynamic(this, &AMETCharacter::WeaponManager_OnChangingWeaponsEvent);
}

UAbilitySystemComponent* AMETCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMETCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*
	 * TODO: Clean up Turn-In-Place
	 */
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_Authority || LocalRole == ROLE_AutonomousProxy)
	{
		RepControlRotation = GetControlRotation();
	}
	
	if (FMath::Abs(ActorControlRotationDelta.Yaw) > 90.f)
	{
		bIsTurningInPlace = true;
	}

	const FVector Velocity = GetCharacterMovement()->Velocity;
	const float GroundSpeed = Velocity.Size2D();
	const bool bIsMoving = GroundSpeed > 3.f && !GetCharacterMovement()->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.f);

	if (bIsTurningInPlace || (bIsMoving && !IsActorControlRotationAligned()))
	{
		FRotator TargetActorRotation = GetActorRotation();
		TargetActorRotation.Yaw = RepControlRotation.Yaw;
		
		const float InterpSpeed = bIsMoving ? 20.f : 10.f;
		FRotator NewActorRotation = FMath::InterpSinInOut(GetActorRotation(), TargetActorRotation, DeltaSeconds * InterpSpeed);
		NewActorRotation.Normalize();
		
		SetActorRotation(NewActorRotation);
	}

	UpdateActorControlRotationDelta();

	if (IsActorControlRotationAligned())
	{
		bIsTurningInPlace = false;
	}

	if (bIsMoving && IsActorControlRotationAligned())
	{
		bUseControllerRotationYaw = true;
	}

	if (!bIsMoving)
	{
		bUseControllerRotationYaw = false;
	}
}

void AMETCharacter::UpdateActorControlRotationDelta()
{
	FRotator Delta = GetActorRotation() - RepControlRotation;
	Delta.Normalize();

	ActorControlRotationDelta.Roll = 0.f;
	ActorControlRotationDelta.Yaw = -Delta.Yaw;

	// Divide by four spine bones (first spine bone used for yaw)
	ActorControlRotationDelta.Pitch = Delta.Pitch / 4.f;
}

bool AMETCharacter::IsActorControlRotationAligned() const
{
	return FMath::Abs(ActorControlRotationDelta.Yaw) < 5.f;
}

void AMETCharacter::AddCharacterAbilities()
{
	if (GetLocalRole() != ROLE_Authority || !AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->AddAbilities(CharacterAbilities);
}

void AMETCharacter::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& InEffectClass, float InLevel) const
{
	if (!ensure(AbilitySystemComponent) || !ensure(InEffectClass)) return;

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InEffectClass, InLevel, ContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AMETCharacter::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultAttributes, 1);
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

void AMETCharacter::OnRep_IsAiming()
{
	SetAiming(bIsAiming);
}

void AMETCharacter::WeaponManager_OnChangingWeaponsEvent(bool bInIsChangingWeapons)
{
	if(bInIsChangingWeapons && bIsAiming)
	{
		SetAiming(false);
	}
}

void AMETCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMETCharacter, RepControlRotation, COND_SkipOwner)
	DOREPLIFETIME_CONDITION(AMETCharacter, bIsAiming, COND_SkipOwner)
	DOREPLIFETIME_CONDITION(AMETCharacter, bIsTurningInPlace, COND_SkipOwner)
}