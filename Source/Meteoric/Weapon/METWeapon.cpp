// Copyright Alex Jobe


#include "METWeapon.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AMETWeapon::AMETWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

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

