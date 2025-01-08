// Copyright Alex Jobe


#include "METInteractableComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

UMETInteractableComponent::UMETInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETInteractableComponent::Interact(AActor* const InSourceActor) const
{
	UGameplayStatics::PlaySoundAtLocation(this, InteractSound, GetOwner()->GetActorLocation());
	InteractEvent.Broadcast(InSourceActor);
}
