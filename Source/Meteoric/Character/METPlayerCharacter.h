// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METCharacter.h"
#include "Navigation/CrowdAgentInterface.h"
#include "METPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETPlayerCharacter : public AMETCharacter, public ICrowdAgentInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> MainCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETFootstepComponent> FootstepComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMCWallRunComponent> WallRunComponent;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> AimAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FireAction;

public:
	/* How far to trace from the camera when determining focal point */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FocalPoint")
    float AimTraceRange;
	
	AMETPlayerCharacter();

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnRep_PlayerState() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End ACharacter interface

	UCameraComponent* GetMainCamera() const { return MainCamera; }

	//~ Begin AMETCharacter interface
	virtual FVector GetFocalPoint() const override;
	virtual FTransform GetEyesPosition() const override;
	//~ End AMETCharacter interface

	//~ Begin ICrowdAgentInterface interface
	virtual FVector GetCrowdAgentLocation() const override;
	virtual FVector GetCrowdAgentVelocity() const override;
	virtual void GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const override;
	virtual float GetCrowdAgentMaxSpeed() const override;
	//~ End ICrowdAgentInterface interface

protected:
	//~ Begin AMETCharacter interface
	virtual void InitAbilityActorInfo() override;
	virtual void Die() override;
	//~ End AMETCharacter interface
	
	void ResetControlRotation() const;
	
private:
	void UnregisterCrowdAgent();
};
