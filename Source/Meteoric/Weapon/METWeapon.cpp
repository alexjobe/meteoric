// Copyright Alex Jobe


#include "METWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Ammo/METWeaponAmmoComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Handling/METRecoilComponent.h"
#include "Handling/METWeaponSpreadComponent.h"
#include "Handling/METWeaponSwayComponent.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/Animation/METAnimationUtils.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"
#include "Meteoric/Interaction/METInteractableComponent.h"
#include "Net/UnrealNetwork.h"
#include "Projectile/METProjectileWeaponComponent.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(EWeaponFiringMode::SingleShot)
	, bStartDropped(true)
	, FiringRate(0.2f)
	, bCanFire(true)
	, bEquipped(false)
	, ElapsedTimeSinceFired(0.f)
	, ElapsedTimeSinceDropped(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	SetWeaponDroppedState(bStartDropped);

	RecoilComponent = CreateDefaultSubobject<UMETRecoilComponent>("RecoilComponent");
	WeaponSwayComponent = CreateDefaultSubobject<UMETWeaponSwayComponent>("WeaponSwayComponent");
	WeaponSpreadComponent = CreateDefaultSubobject<UMETWeaponSpreadComponent>("WeaponSpreadComponent");
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
	WeaponSpreadComponent->OnWeaponEquipped(OwningCharacter);
	AmmoComponent->OnWeaponEquipped(OwningCharacter);
	
	if (GetLocalRole() == ROLE_Authority && !ActiveEquippedEffectHandle.IsSet())
	{
		ActiveEquippedEffectHandle = UMETAbilitySystemUtils::ApplyEffectClassToActor(OwningCharacter, this, EquippedEffectClass, 1);
	}
	
	SetActorTickEnabled(true);

	bEquipped = true;
}

void AMETWeapon::OnUnequipped()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	
	FinishReload(false);
	SetActorTickEnabled(false);
	FinishUnequip();
}

void AMETWeapon::Drop()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	FinishReload(false);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetWeaponDroppedState(true);
	ElapsedTimeSinceDropped = 0.f;
	FinishUnequip();
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

inline void AMETWeapon::FinishUnequip()
{
	if (GetLocalRole() == ROLE_Authority && ActiveEquippedEffectHandle.IsSet())
	{
		UMETAbilitySystemUtils::RemoveEffectFromActor(OwningCharacter, ActiveEquippedEffectHandle.GetValue());
		ActiveEquippedEffectHandle.Reset();
	}
	
	OwningCharacter = nullptr;
	RecoilComponent->Reset();
	WeaponSwayComponent->Reset();
	WeaponSpreadComponent->OnWeaponUnequipped();
	AmmoComponent->OnWeaponUnequipped();
	bCanFire = true;
	bEquipped = false;
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
		Mesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
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

void AMETWeapon::OnAimDownSights(const bool bInIsAiming) const
{
	if(ensure(WeaponSwayComponent) && ensure(WeaponSpreadComponent))
	{
		WeaponSwayComponent->OnAimDownSights(bInIsAiming);
		WeaponSpreadComponent->OnAimDownSights(bInIsAiming);
	}
}

void AMETWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AMETWeapon::StartFire(const bool bInHeld)
{
	if(!bCanFire) return;
	if(bInHeld && FiringMode != EWeaponFiringMode::Automatic) return;

	if (!ensure(AmmoComponent) || !AmmoComponent->TryConsumeAmmo()) return;

	bCanFire = false;
	ElapsedTimeSinceFired = 0.f;
	
	if(ensure(OwningCharacter) && AnimationSettings.WeaponFireMontage)
	{
		OwningCharacter->PlayAnimMontage(AnimationSettings.CharacterFireMontage);
		UMETAnimationUtils::PlayAnimMontage(Mesh, AnimationSettings.WeaponFireMontage);
	}
}

void AMETWeapon::FinishFire(const bool bInHeld) const
{
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
	if (AnimationSettings.WeaponReloadMontage)
	{
		UMETAnimationUtils::PlayAnimMontage(Mesh, AnimationSettings.WeaponReloadMontage);
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
	if(!ensure(Mesh) || !ensure(RecoilComponent) || !ensure(WeaponSwayComponent) || !ensure(WeaponSpreadComponent)) return;

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
	WeaponSpreadComponent->UpdateWeaponSpread(DeltaTime);

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
	if (!OwningCharacter) return;
	if (OwningCharacter->GetLocalRole() == ROLE_SimulatedProxy)
	{
		OnReloadEvent.Broadcast(bIsReloading);
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

