// Copyright Alex Jobe


#include "METInteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "METInteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"

UMETInteractionComponent::UMETInteractionComponent()
	: LineTraceDistance(200.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETInteractionComponent::InitializeComponent()
{
	OwningCharacter = CastChecked<ACharacter>(GetOwner());
	CameraComponent = OwningCharacter->FindComponentByClass<UCameraComponent>();
}

void UMETInteractionComponent::Interact(const FMinimalViewInfo& InViewInfo) const
{
	if(UMETInteractableComponent* InteractableComponent = FindInteractableComponent(InViewInfo))
	{
		InteractableComponent->Interact(OwningCharacter);
		InteractEvent.Broadcast(InteractableComponent->GetOwner());
	}
}

void UMETInteractionComponent::Server_Interact_Implementation(const FMinimalViewInfo& InViewInfo)
{
	Interact(InViewInfo);
}

UMETInteractableComponent* UMETInteractionComponent::FindInteractableComponent(const FMinimalViewInfo& InViewInfo) const
{
	const FVector TraceStart = InViewInfo.Location;
	const FVector TraceEnd = TraceStart + InViewInfo.Rotation.Vector() * LineTraceDistance;

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwningCharacter);
	UKismetSystemLibrary::SphereTraceSingle(this, TraceStart, TraceEnd, 20.f, UEngineTypes::ConvertToTraceType(ECC_Interaction), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	if(const AActor* HitActor = HitResult.GetActor())
	{
		return HitActor->FindComponentByClass<UMETInteractableComponent>();
	}

	return nullptr;
}

void UMETInteractionComponent::InteractInput()
{
	if(!ensure(CameraComponent)) return;

	// TODO: Implement cooldown
	
	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(GetWorld()->DeltaTimeSeconds, ViewInfo);
	
	if(GetOwner()->HasAuthority())
	{
		Interact(ViewInfo);
	}
	else
	{
		Server_Interact(ViewInfo);
	}
}

void UMETInteractionComponent::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UMETInteractionComponent::InteractInput);
	}
}
