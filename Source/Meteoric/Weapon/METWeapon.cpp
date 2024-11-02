// Copyright Alex Jobe


#include "METWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
	, FiringMode(SingleShot)
	, FiringRate(0.2f)
	, LastTimeFired(0.f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AMETWeapon::OnEquipped(ACharacter* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
}

void AMETWeapon::OnFireStarted()
{
	if(FiringMode == SingleShot)
	{
		Fire();	
	}
}

void AMETWeapon::OnFireHeld()
{
	if(FiringMode == Automatic)
	{
		FRotator ControlRotation = OwningCharacter->GetController()->GetControlRotation();
		ControlRotation += FRotator(0.2f, 0.f, 0.f);
		OwningCharacter->GetController()->SetControlRotation(ControlRotation);
		Fire();
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
}

void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

