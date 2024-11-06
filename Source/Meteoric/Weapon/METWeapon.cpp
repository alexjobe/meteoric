// Copyright Alex Jobe


#include "METWeapon.h"

#include "METRecoilComponent.h"
#include "METWeaponSwayComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, FiringRate(0.2f)
	, LastTimeFired(0.f)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetReplicates(true);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	RecoilComponent = CreateDefaultSubobject<UMETRecoilComponent>("RecoilComponent");
	WeaponSwayComponent = CreateDefaultSubobject<UMETWeaponSwayComponent>("WeaponSwayComponent");
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	OwningCharacter = InOwningCharacter;
	SetOwner(OwningCharacter->GetOwner());
	RecoilComponent->OnWeaponEquipped(OwningCharacter, FiringMode);
	WeaponSwayComponent->OnWeaponEquipped(OwningCharacter);
	SetActorTickEnabled(true);

	if(HasAuthority())
	{
		Multicast_OnEquipped(InOwningCharacter);
	}
}

void AMETWeapon::OnUnequipped()
{
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;
	SetActorTickEnabled(false);
	OwningCharacter = nullptr;
	RecoilComponent->Reset();
	WeaponSwayComponent->Reset();

	if(HasAuthority())
	{
		Multicast_OnUnequipped();
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

void AMETWeapon::Multicast_OnEquipped_Implementation(ACharacter* InOwningCharacter)
{
	if(!HasAuthority())
	{
		OnEquipped(InOwningCharacter);
	}
}

void AMETWeapon::Multicast_OnUnequipped_Implementation()
{
	if(!HasAuthority())
	{
		OnUnequipped();
	}
}

void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!ensure(RecoilComponent) || !ensure(WeaponSwayComponent)) return;

	RecoilComponent->UpdateRecoil(DeltaTime);
	WeaponSwayComponent->UpdateWeaponSway(DeltaTime);
}

