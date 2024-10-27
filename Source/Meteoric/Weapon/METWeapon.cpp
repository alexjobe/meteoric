// Copyright Alex Jobe


#include "METWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AMETWeapon::AMETWeapon()
	: SightCameraOffset(30.f)
	, AimDownSightsSpeed(20.f)
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

void AMETWeapon::Fire() const
{
	if(ensure(OwningCharacter))
	{
		OwningCharacter->PlayAnimMontage(CharacterFireMontage);
	}
}

// Called when the game starts or when spawned
void AMETWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMETWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

