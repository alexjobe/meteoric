// Copyright Alex Jobe


#include "METWeapon.h"

#include "METRecoilComponent.h"
#include "METWeaponSwayComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/Interaction/METInteractableComponent.h"
#include "Net/UnrealNetwork.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, FiringRate(0.2f)
	, LastTimeFired(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetIsReplicated(true);
	SetReplicatingMovement(true);

	SetWeaponDropped(true);

	RecoilComponent = CreateDefaultSubobject<UMETRecoilComponent>("RecoilComponent");
	WeaponSwayComponent = CreateDefaultSubobject<UMETWeaponSwayComponent>("WeaponSwayComponent");
	InteractableComponent = CreateDefaultSubobject<UMETInteractableComponent>("InteractableComponent");
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	if(!InOwningCharacter) return;
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	OwningCharacter = InOwningCharacter;
	RecoilComponent->OnWeaponEquipped(OwningCharacter, FiringMode);
	WeaponSwayComponent->OnWeaponEquipped(OwningCharacter);
	SetActorTickEnabled(true);
}

void AMETWeapon::OnUnequipped()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	SetActorTickEnabled(false);
	OwningCharacter = nullptr;
	RecoilComponent->Reset();
	WeaponSwayComponent->Reset();
}

void AMETWeapon::OnAimDownSights(bool bInIsAiming) const
{
	if(ensure(WeaponSwayComponent))
	{
		WeaponSwayComponent->OnAimDownSights(bInIsAiming);
	}
}

void AMETWeapon::Drop()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetWeaponDropped(true);
	//OnUnequipped();

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

void AMETWeapon::SetWeaponDropped(bool bInDropped)
{
	if(!ensure(Mesh)) return;
	
	if(bInDropped)
	{
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Mesh->SetCollisionObjectType(ECC_WorldDynamic);
		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Mesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);
		Mesh->SetEnableGravity(true);
		Mesh->SetSimulatePhysics(true);
	}
	else
	{
		Mesh->SetEnableGravity(false);
		Mesh->SetSimulatePhysics(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
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
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;

	// Note: If performance is a concern, we can just update recoil on the locally-controlled pawn
	RecoilComponent->UpdateRecoil(DeltaTime);
	WeaponSwayComponent->UpdateWeaponSway(DeltaTime);
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

void AMETWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMETWeapon, OwningCharacter)
}

