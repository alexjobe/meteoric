// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Interact Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> InteractAction;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float LineTraceDistance;

	/* How long the player has to wait between triggering interactions */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionCooldown;
	
	UMETInteractionComponent();

	virtual void InitializeComponent() override;

	void Interact(const FMinimalViewInfo& InViewInfo) const;

private:
	UFUNCTION(Server, Reliable)
	void Server_Interact(const FMinimalViewInfo& InViewInfo);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractEvent, AActor*, Interactable);
	FInteractEvent& OnInteractEvent() { return InteractEvent; }

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient)
	TObjectPtr<class UCameraComponent> CameraComponent;

	float LastTimeInteracted;

	FInteractEvent InteractEvent;

	class UMETInteractableComponent* FindInteractableComponent(const FMinimalViewInfo& InViewInfo) const;

	/** Called for interact input */
	void InteractInput();

public:
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
	
};
