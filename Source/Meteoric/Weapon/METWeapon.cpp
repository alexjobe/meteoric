// Copyright Alex Jobe


#include "METWeapon.h"

#include "METRecoilComponent.h"
#include "METWeaponSwayComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Meteoric/Interaction/METInteractableComponent.h"
#include "Net/UnrealNetwork.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, FiringRate(0.2f)
	, LastTimeFired(0.f)
	, ElapsedTimeSinceDropped(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	SetWeaponDroppedState(true);

	RecoilComponent = CreateDefaultSubobject<UMETRecoilComponent>("RecoilComponent");
	WeaponSwayComponent = CreateDefaultSubobject<UMETWeaponSwayComponent>("WeaponSwayComponent");
	InteractableComponent = CreateDefaultSubobject<UMETInteractableComponent>("InteractableComponent");
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	if(!IsValid(InOwningCharacter)) return;
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	OwningCharacter = InOwningCharacter;
	RecoilComponent->OnWeaponEquipped(OwningCharacter, FiringMode);
	WeaponSwayComponent->OnWeaponEquipped(OwningCharacter);
	if (GetLocalRole() == ROLE_Authority && !EquippedEffectHandle.IsSet())
	{
		EquippedEffectHandle = UMETAbilitySystemUtils::ApplyEffectToActor(OwningCharacter, this, EquippedEffectClass, 1);
	}
	SetActorTickEnabled(true);
}

void AMETWeapon::OnUnequipped()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	SetActorTickEnabled(false);
	RemoveOwningCharacter();
}

void AMETWeapon::Drop()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetWeaponDroppedState(true);
	ElapsedTimeSinceDropped = 0.f;
	RemoveOwningCharacter();

	if(HasAuthority())
	{
		Multicast_Drop();
	}
}

void AMETWeapon::Multicast_Drop_Implementation()
{
	if(!HasAuthority())
	{
		Drop();
	}
}

inline void AMETWeapon::RemoveOwningCharacter()
{
	if (GetLocalRole() == ROLE_Authority && EquippedEffectHandle.IsSet())
	{
		UMETAbilitySystemUtils::RemoveEffectFromActor(OwningCharacter, EquippedEffectHandle.GetValue());
		EquippedEffectHandle.Reset();
	}
	OwningCharacter = nullptr;
	RecoilComponent->Reset();
	WeaponSwayComponent->Reset();
}

void AMETWeapon::SetWeaponDroppedState(bool bInDropped)
{
	if(!ensure(Mesh)) return;
	
	if(bInDropped)
	{
		SetWeaponPhysicsEnabled(true);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionObjectType(ECC_WorldDynamic);
		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);
	}
	else
	{
		SetWeaponPhysicsEnabled(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void AMETWeapon::OnAimDownSights(bool bInIsAiming) const
{
	if(ensure(WeaponSwayComponent))
	{
		WeaponSwayComponent->OnAimDownSights(bInIsAiming);
	}
}

void AMETWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AMETWeapon::Fire(bool bInHeld)
{
	if(bInHeld && FiringMode != Automatic) return;
	
	const float TimeSinceCreation = GetGameTimeSinceCreation();
	if(TimeSinceCreation - LastTimeFired < FiringRate) return;
	LastTimeFired = TimeSinceCreation;
	
	if(ensure(OwningCharacter) && WeaponFireAnim)
	{
		OwningCharacter->PlayAnimMontage(CharacterFireMontage);
		Mesh->PlayAnimation(WeaponFireAnim, false);
	}

	if(ensure(RecoilComponent))
	{
		RecoilComponent->OnWeaponFired();
		if(bInHeld)
		{
			RecoilComponent->OnFireActionHeld();
		}
		else
		{
			RecoilComponent->OnFireActionStarted();
		}
	}
}

bool AMETWeapon::CanFire() const
{
	const float TimeSinceCreation = GetGameTimeSinceCreation();
	return TimeSinceCreation - LastTimeFired >= FiringRate;
}

void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!ensure(Mesh) || !ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;

	// Note: If performance is a concern, we can just update recoil on the locally-controlled pawn
	RecoilComponent->UpdateRecoil(DeltaTime);
	WeaponSwayComponent->UpdateWeaponSway(DeltaTime);

	if(Mesh->IsSimulatingPhysics())
	{
		// We only need physics enabled when we drop the weapon. Once it hits the ground and stops moving, we can turn
		// off physics and stop ticking
		ElapsedTimeSinceDropped += DeltaTime;
		if(Mesh->GetComponentVelocity().IsNearlyZero() && ElapsedTimeSinceDropped > 5.f)
		{
			SetWeaponPhysicsEnabled(false);
			SetActorTickEnabled(false);
		}
	}
}

void AMETWeapon::OnRep_OwningCharacter(ACharacter* InOldOwner)
{
	if(InOldOwner == OwningCharacter) return;

	if(OwningCharacter)
	{
		OnEquipped(OwningCharacter);
	}
	else
	{
		OnUnequipped();
	}
}

void AMETWeapon::SetWeaponPhysicsEnabled(bool bInEnabled)
{
	if(!ensure(Mesh)) return;
	SetReplicatingMovement(bInEnabled);
	Mesh->SetIsReplicated(bInEnabled);
	Mesh->SetEnableGravity(bInEnabled);
	Mesh->SetSimulatePhysics(bInEnabled);
}

void AMETWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMETWeapon, OwningCharacter)
}

