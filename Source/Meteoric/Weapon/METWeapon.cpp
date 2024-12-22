// Copyright Alex Jobe


#include "METWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "METProjectileWeaponComponent.h"
#include "METRecoilComponent.h"
#include "METWeaponSwayComponent.h"
#include "Ammo/METWeaponAmmoComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/Animation/METAnimationUtils.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Meteoric/Interaction/METInteractableComponent.h"
#include "Net/UnrealNetwork.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, Damage(25.f)
	, FiringRate(0.2f)
	, bCanFire(true)
	, ElapsedTimeSinceFired(0.f)
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
	ProjectileWeaponComponent = CreateDefaultSubobject<UMETProjectileWeaponComponent>("ProjectileWeaponComponent");
	AmmoComponent = CreateDefaultSubobject<UMETWeaponAmmoComponent>("AmmoComponent");
	AmmoComponent->SetIsReplicated(true);
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	if(!IsValid(InOwningCharacter)) return;
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent) || !ensure(AmmoComponent)) return;
	OwningCharacter = InOwningCharacter;
	RecoilComponent->OnWeaponEquipped(OwningCharacter, FiringMode);
	WeaponSwayComponent->OnWeaponEquipped(OwningCharacter);
	AmmoComponent->OnWeaponEquipped(OwningCharacter);
	if (GetLocalRole() == ROLE_Authority && !EquippedEffectHandle.IsSet())
	{
		EquippedEffectHandle = UMETAbilitySystemUtils::ApplyEffectClassToActor(OwningCharacter, this, EquippedEffectClass, 1);
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
	GetMesh()->SetVisibility(true, true);

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
	AmmoComponent->OnWeaponUnequipped();
	bCanFire = true;
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
	if(!bCanFire) return;
	if(bInHeld && FiringMode != Automatic) return;

	if (!ensure(AmmoComponent) || !AmmoComponent->TryConsumeAmmo()) return;

	bCanFire = false;
	ElapsedTimeSinceFired = 0.f;
	
	if(ensure(OwningCharacter) && WeaponFireMontage)
	{
		OwningCharacter->PlayAnimMontage(CharacterFireMontage);
		UMETAnimationUtils::PlayAnimMontage(Mesh, WeaponFireMontage);
	}

	if(ensure(RecoilComponent))
	{
		RecoilComponent->OnWeaponFired();
		if(bInHeld)
		{
			RecoilComponent->OnFireActionHeld();
		}
	}
}

bool AMETWeapon::CanFire() const
{
	if(!ensure(AmmoComponent)) return false;
	return AmmoComponent->GetAmmoCount() > 0 && bCanFire;
}

void AMETWeapon::StartReload() const
{
	OnReloadEvent.Broadcast(true);
	if (WeaponReloadMontage)
	{
		UMETAnimationUtils::PlayAnimMontage(Mesh, WeaponReloadMontage);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		Multicast_OnReload(true);
	}
}

void AMETWeapon::FinishReload(const bool bSuccess) const
{
	if (bSuccess && ensure(AmmoComponent))
	{
		AmmoComponent->Reload();
	}

	OnReloadEvent.Broadcast(false);

	if (GetLocalRole() == ROLE_Authority)
	{
		Multicast_OnReload(false);
	}
}

void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!ensure(Mesh) || !ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;

	if (!bCanFire)
	{
		ElapsedTimeSinceFired += DeltaTime;
		if (ElapsedTimeSinceFired >= FiringRate)
		{
			bCanFire = true;
			FGameplayEventData EventData;
			EventData.EventTag = METGameplayTags::EventTag_FireCooldown;
			EventData.Instigator = this;
			EventData.Target = OwningCharacter;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwningCharacter, METGameplayTags::EventTag_FireCooldown, EventData);
		}
	}

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

void AMETWeapon::Multicast_OnReload_Implementation(const bool bIsReloading) const
{
	if (!ensure(OwningCharacter)) return;
	if (OwningCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		OnReloadEvent.Broadcast(bIsReloading);
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

