// Copyright Alex Jobe


#include "METCharacter.h"

#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Meteoric/GAS/METAttributeSet.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"
#include "Net/UnrealNetwork.h"
#include "PMCoverSystem/Public/Components/PMCoverUserComponent.h"

DEFINE_LOG_CATEGORY(LogMETCharacter);

static TAutoConsoleVariable<int32> CVarDrawCharacterRotationDebug(
	TEXT("METCharacter.DrawRotationDebug"),
	0,
	TEXT("If 1, draws actor rotation and aim rotation")
);

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
	GetMesh()->SetGenerateOverlapEvents(true);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	WeaponManager = CreateDefaultSubobject<UMETWeaponManager>(TEXT("WeaponManager"));
	WeaponManager->OnChangingWeaponsEvent().AddUniqueDynamic(this, &AMETCharacter::WeaponManager_OnChangingWeaponsEvent);

	AmmoManager = CreateDefaultSubobject<UMETAmmoManager>(TEXT("AmmoManager"));
	CoverUserComponent = CreateDefaultSubobject<UPMCoverUserComponent>(TEXT("CoverUserComponent"));
	CoverUserComponent->InitializeCoverUser(GetCapsuleComponent());
}

UAbilitySystemComponent* AMETCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMETCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		MovementComponent->NavAgentProps.bCanCrouch = true;
		MovementComponent->SetCrouchedHalfHeight(60.f);
	}

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

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarDrawCharacterRotationDebug.GetValueOnAnyThread() == 1)
	{
		DrawCharacterRotationDebug();
	}
#endif
}

void AMETCharacter::UpdateCharacterAimRotation(float DeltaSeconds)
{
	if (IsDead()) return;
	
	const ENetRole LocalRole = GetLocalRole();
	if (LocalRole == ROLE_Authority || LocalRole == ROLE_AutonomousProxy)
	{
		RepControlRotation = GetControlRotation();
	}

	// Interp aim rotation towards control rotation, to prevent "snapping"
	AimRotation = FMath::Lerp(AimRotation, RepControlRotation, DeltaSeconds * CharacterConfig.AimInterpSpeed);
	
	if (FMath::Abs(ActorAimRotationDelta.Yaw) > 90.f)
	{
		bIsTurningInPlace = true;
	}
	
	const bool bIsMoving = IsMoving();

	if (bIsTurningInPlace || (bIsMoving && !IsActorControlRotationAligned()))
	{
		FRotator TargetActorRotation = GetActorRotation();
		TargetActorRotation.Yaw = AimRotation.Yaw;
		
		const float InterpSpeed = bIsMoving ? CharacterConfig.TurnInPlaceSpeed_Moving : CharacterConfig.TurnInPlaceSpeed_Stationary;
		FRotator NewActorRotation = FMath::InterpSinInOut(GetActorRotation(), TargetActorRotation, DeltaSeconds * InterpSpeed);
		NewActorRotation.Normalize();
		
		SetActorRotation(NewActorRotation);
	}
	
	UpdateActorAimRotationDelta();

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

void AMETCharacter::UpdateActorAimRotationDelta()
{
	FRotator Delta = GetActorRotation() - AimRotation;
	Delta.Normalize();

	ActorAimRotationDelta.Roll = 0.f;
	ActorAimRotationDelta.Yaw = -Delta.Yaw;
	ActorAimRotationDelta.Pitch = Delta.Pitch;
}

bool AMETCharacter::IsActorControlRotationAligned() const
{
	return FMath::Abs(ActorAimRotationDelta.Yaw) < 5.f;
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
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AMETCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X * .4f);
		AddControllerPitchInput(LookAxisVector.Y * .4f);
	}
}

void AMETCharacter::FireWeapon(const bool bInHeld)
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

FVector AMETCharacter::GetFocalPoint() const
{
	return GetActorLocation();
}

FTransform AMETCharacter::GetEyesPosition() const
{
	if (const USkeletalMeshComponent* SkeletalMesh = GetMesh(); ensure(SkeletalMesh))
	{
		return SkeletalMesh->GetBoneTransform(FName("S_Camera"));
	}
	return FTransform();
}

void AMETCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	if (IGenericTeamAgentInterface* Interface = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		Interface->SetGenericTeamId(InTeamID);
	}
}

FGenericTeamId AMETCharacter::GetGenericTeamId() const
{
	if (const IGenericTeamAgentInterface* Interface = Cast<IGenericTeamAgentInterface>(GetController()))
	{
		return Interface->GetGenericTeamId();
	}
	return FGenericTeamId();
}

void AMETCharacter::Die()
{
	RemoveCharacterAbilities();

	if (UCapsuleComponent* Capsule = GetCapsuleComponent(); ensure(Capsule))
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (USkeletalMeshComponent* MeshComponent = GetMesh(); ensure(MeshComponent))
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement(); ensure(MovementComponent))
	{
		MovementComponent->GravityScale = 0.f;
		MovementComponent->Velocity = FVector::ZeroVector;
	}
	
	AimRotation = FRotator::ZeroRotator;
	ActorAimRotationDelta = FRotator::ZeroRotator;
	bIsTurningInPlace = false;

	if (ensure(WeaponManager))
	{
		WeaponManager->DropAllWeapons();
	}

	if (ensure(CoverUserComponent))
	{
		CoverUserComponent->ReleaseCoverSpots();
	}
	
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(METGameplayTags::State_Dead);
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
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

void AMETCharacter::SetAiming(const bool bInIsAiming)
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
	const AMETWeapon* const CurrentWeapon = WeaponManager->GetCurrentWeapon();
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

void AMETCharacter::AttributeSet_OnAttributeDamageEvent(const float DamageAmount, const FGameplayEffectSpec& EffectSpec)
{
	// Empty in base class
}

void AMETCharacter::BindAttributeChangedCallbacks()
{
	if (!ensure(AbilitySystemComponent) || !ensure(AttributeSet)) return;
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	AttributeSet->OnAttributeDamageEvent().AddUObject(this, &AMETCharacter::AttributeSet_OnAttributeDamageEvent);
}

void AMETCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	if (!ensure(AbilitySystemComponent)) return;
	
	const float Health = Data.NewValue;
	if (Health <= 0.0f && !AbilitySystemComponent->HasMatchingGameplayTag(METGameplayTags::State_Dead))
	{
		Die();
	}
}

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
void AMETCharacter::DrawCharacterRotationDebug() const
{
	// Actor Rotation
	const FVector LineStart = GetActorLocation();
	FVector LineEnd = LineStart + GetActorForwardVector() * 150.f;;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Blue, false, -1, 0, 2.f);

	// Control Rotation
	LineEnd = LineStart + RepControlRotation.Vector() * 150.f;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red, false, -1, 0, 2.f);

	// Aim Rotation
	LineEnd = LineStart + AimRotation.Vector() * 150.f;
	DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, false, -1, 0, 2.f);
}
#endif

void AMETCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMETCharacter, RepControlRotation, COND_SkipOwner)
	DOREPLIFETIME_CONDITION(AMETCharacter, bIsAiming, COND_SkipOwner)
	DOREPLIFETIME_CONDITION(AMETCharacter, bIsTurningInPlace, COND_SkipOwner)
}
