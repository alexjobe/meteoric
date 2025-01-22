// Copyright Alex Jobe


#include "METCharacter.h"

#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/GameMode/METGameModeBase.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"
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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);

	WeaponManager = CreateDefaultSubobject<UMETWeaponManager>(TEXT("WeaponManager"));
	WeaponManager->OnChangingWeaponsEvent().AddUniqueDynamic(this, &AMETCharacter::WeaponManager_OnChangingWeaponsEvent);

	AmmoManager = CreateDefaultSubobject<UMETAmmoManager>(TEXT("AmmoManager"));
}

UAbilitySystemComponent* AMETCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMETCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Make sure the mesh is updated on server so projectiles spawn in the correct location
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
}

void AMETCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacterAimRotation(DeltaSeconds);
}

void AMETCharacter::UpdateCharacterAimRotation(float DeltaSeconds)
{
	if (IsDead()) return;
	
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_Authority || LocalRole == ROLE_AutonomousProxy)
	{
		RepControlRotation = GetControlRotation();
	}
	
	if (FMath::Abs(ActorControlRotationDelta.Yaw) > 90.f)
	{
		bIsTurningInPlace = true;
	}
	
	const bool bIsMoving = IsMoving();

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
	ActorControlRotationDelta.Pitch = Delta.Pitch;
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

	AbilitySystemComponent->AddAbilities(CharacterAbilities, this);
	AbilitySystemComponent->bCharacterAbilitiesGiven = true;
}

void AMETCharacter::RemoveCharacterAbilities() const
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const auto& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if (Spec.SourceObject == this && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	for (int Index = 0; Index < AbilitiesToRemove.Num(); Index++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[Index]);
	}

	AbilitySystemComponent->bCharacterAbilitiesGiven = false;
}

void AMETCharacter::InitAbilityActorInfo()
{
}

void AMETCharacter::InitializeDefaultAttributes() const
{
	UMETAbilitySystemUtils::ApplyEffectClassToActor(this, this, DefaultMaxAttributes, 1);
	UMETAbilitySystemUtils::ApplyEffectClassToActor(this, this, DefaultAttributes, 1);
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

void AMETCharacter::FireWeapon(bool bInHeld)
{
	if(AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon())
	{
		CurrentWeapon->Fire(bInHeld);
	}

	if(HasAuthority())
	{
		Multicast_FireWeapon(bInHeld);
	}
}

FTransform AMETCharacter::GetEyesViewpoint() const
{
	return GetActorTransform();
}

void AMETCharacter::Die()
{
	RemoveCharacterAbilities();
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	RepControlRotation = FRotator::ZeroRotator;
	ActorControlRotationDelta = FRotator::ZeroRotator;
	bIsTurningInPlace = false;
	
	WeaponManager->DropAllWeapons();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(METGameplayTags::State_Dead);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	if (HasAuthority())
	{
		if (AMETGameModeBase* GameMode = Cast<AMETGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->PlayerDied(GetController());
		}
	}
}

void AMETCharacter::Multicast_FireWeapon_Implementation(bool bInHeld)
{
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_SimulatedProxy)
	{
		FireWeapon(bInHeld);
	}
}

AMETWeapon* AMETCharacter::GetWeapon() const
{
	return WeaponManager ? WeaponManager->GetCurrentWeapon() : nullptr;
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

bool AMETCharacter::IsMoving() const
{
	const FVector Velocity = GetCharacterMovement()->Velocity;
	const float GroundSpeed = Velocity.Size2D();
	return GroundSpeed > 3.f && !GetCharacterMovement()->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0.f);
}

bool AMETCharacter::IsDead() const
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(METGameplayTags::State_Dead);
	}
	return false;
}

bool AMETCharacter::ShouldEnableLeftHandIK() const
{
	if (!AbilitySystemComponent) return false;

	FGameplayTagContainer DisableTags;
	DisableTags.AddTag(METGameplayTags::Ability_ReloadWeapon);
	DisableTags.AddTag(METGameplayTags::Ability_CycleAmmo);

	return GetWeapon() && !AbilitySystemComponent->HasAnyMatchingGameplayTags(DisableTags);
}

bool AMETCharacter::CanFireWeapon() const
{
	if(!ensure(WeaponManager)) return false;
	AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon();
	return CurrentWeapon && CurrentWeapon->CanFire() && !WeaponManager->IsChangingWeapons();
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