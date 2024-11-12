// Copyright Alex Jobe


#include "METInteractableComponent.h"

UMETInteractableComponent::UMETInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETInteractableComponent::Interact(AActor* const InSourceActor) const
{
	InteractEvent.Broadcast(InSourceActor);
}
