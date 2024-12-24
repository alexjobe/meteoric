// Copyright Alex Jobe


#include "METAmmoPickup.h"

#include "METAmmoManager.h"
#include "Components/SphereComponent.h"
#include "Meteoric/Meteoric.h"
#include "Meteoric/Interaction/METInteractableComponent.h"


AMETAmmoPickup::AMETAmmoPickup()
	: AmmoCount(100.f)
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	check(Mesh)
	RootComponent = Mesh;
	
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("CollisionComponent");
	check(CollisionComponent)
	CollisionComponent->SetupAttachment(RootComponent);
	CollisionComponent->InitSphereRadius(10.f);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Interaction, ECR_Block);

	InteractableComponent = CreateDefaultSubobject<UMETInteractableComponent>("InteractableComponent");
	check(InteractableComponent)

	InteractableComponent->OnInteractEvent().AddUniqueDynamic(this, &AMETAmmoPickup::InteractableComponent_OnInteractEvent);
}

void AMETAmmoPickup::InteractableComponent_OnInteractEvent(AActor* const InSourceActor)
{
	if (!ensure(InSourceActor)) return;

	UMETAmmoManager* AmmoManager = InSourceActor->FindComponentByClass<UMETAmmoManager>();
	if (!ensure(AmmoManager)) return;

	AmmoManager->AddReserveAmmo(AmmoType, AmmoCount);
	Destroy();
}
