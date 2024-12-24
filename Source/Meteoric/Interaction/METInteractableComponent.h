// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METInteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMETInteractableComponent();

	void Interact(AActor* const InSourceActor) const;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractEvent, AActor* const, SourceActor);
	FInteractEvent& OnInteractEvent() { return InteractEvent; }

private:
	FInteractEvent InteractEvent;
};
