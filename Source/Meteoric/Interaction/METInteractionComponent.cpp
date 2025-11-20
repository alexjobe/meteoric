// Copyright Alex Jobe


#include "METInteractionComponent.h"

#include "EnhancedInputComponent.h"
#include "METInteractableComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Meteoric/Meteoric.h"

UMETInteractionComponent::UMETInteractionComponent()
	: TraceDistance(200.f)
	, SphereTraceRadius(30.f)
	, InteractionCooldown(.5f)
	, DrawDebugTrace(EDrawDebugTrace::None)
	, LastTimeInteracted(0.f)
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
	if(const UMETInteractableComponent* InteractableComponent = FindInteractableComponent(InViewInfo))
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
	const FVector TraceEnd = TraceStart + InViewInfo.Rotation.Vector() * TraceDistance;

	FHitResult HitResult;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwningCharacter);
	
	UKismetSystemLibrary::SphereTraceSingle(this, TraceStart, TraceEnd, SphereTraceRadius, UEngineTypes::ConvertToTraceType(ECC_Interaction), false, ActorsToIgnore, DrawDebugTrace, HitResult, true);
	
	if(const AActor* HitActor = HitResult.GetActor())
	{
		return HitActor->FindComponentByClass<UMETInteractableComponent>();
	}

	return nullptr;
}

void UMETInteractionComponent::InteractInput()
{
	if(!ensure(CameraComponent)) return;

	const float GameTimeSeconds = GetWorld()->GetTimeSeconds();
	if(GameTimeSeconds - LastTimeInteracted < InteractionCooldown) return;
	LastTimeInteracted = GameTimeSeconds;
	
	FMinimalViewInfo ViewInfo;
	CameraComponent->GetCameraView(GetWorld()->DeltaTimeSeconds, ViewInfo);
	
	if(GetOwner()->HasAuthority())
	{
		Interact(ViewInfo);
	}
	else
	{
		Interact(ViewInfo);
		Server_Interact(ViewInfo);
	}
}

void UMETInteractionComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &UMETInteractionComponent::InteractInput);
	}
}
