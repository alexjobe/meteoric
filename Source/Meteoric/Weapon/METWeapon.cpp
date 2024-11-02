// Copyright Alex Jobe


#include "METWeapon.h"

#include "METRecoilComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, FiringRate(0.2f)
	, LastTimeFired(0.f)
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	RecoilComponent = CreateDefaultSubobject<UMETRecoilComponent>("RecoilComponent");
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	if(!ensure(RecoilComponent)) return;
	OwningCharacter = InOwningCharacter;
	RecoilComponent->OnWeaponEquipped(OwningCharacter);
}

void AMETWeapon::OnFireActionStarted()
{
	if(FiringMode == SingleShot)
	{
		Fire();	
	}
	
	if(ensure(RecoilComponent))
	{
		RecoilComponent->OnFireActionStarted();
	}
}

void AMETWeapon::OnFireActionHeld()
{
	if(FiringMode == Automatic)
	{
		Fire();
	}

	if(ensure(RecoilComponent))
	{
		RecoilComponent->OnFireActionHeld();
	}
}

void AMETWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AMETWeapon::Fire()
{
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
	}
}

void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

