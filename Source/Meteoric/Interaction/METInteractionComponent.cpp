// Copyright Alex Jobe


#include "METInteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "METInteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Meteoric/Meteoric.h"

UMETInteractionComponent::UMETInteractionComponent()
	: LineTraceDistance(300.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETInteractionComponent::InitializeComponent()
{
	OwningCharacter = CastChecked<ACharacter>(GetOwner());
	CameraComponent = OwningCharacter->FindComponentByClass<UCameraComponent>();
}

void UMETInteractionComponent::Interact() const
{
	if(UMETInteractableComponent* InteractableComponent = FindInteractableComponent())
	{
		InteractableComponent->Interact(OwningCharacter);
		InteractEvent.Broadcast(InteractableComponent->GetOwner());
	}
}

UMETInteractableComponent* UMETInteractionComponent::FindInteractableComponent() const
{
	if(!ensure(CameraComponent)) return nullptr;

	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(GetWorld()->DeltaTimeSeconds, ViewInfo);

	const FVector TraceStart = ViewInfo.Location;
	const FVector TraceEnd = TraceStart + ViewInfo.Rotation.Vector() * LineTraceDistance;

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5.f);

	FHitResult HitResult;

	if(GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Interaction))
	{
		if(AActor* HitActor = HitResult.GetActor())
		{
			return HitActor->FindComponentByClass<UMETInteractableComponent>();
		}
	}

	return nullptr;
}

void UMETInteractionComponent::InteractInput()
{
	Interact();
}

void UMETInteractionComponent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UMETInteractionComponent::InteractInput);
	}
}
