// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "METCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMETCharacter, Log, All);

UCLASS(config=Game)
class METEORIC_API AMETCharacter : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> MainCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETWeaponManager> WeaponManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETInteractionComponent> InteractionComponent;

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
	AMETCharacter();

	virtual void Tick(float DeltaSeconds) override;
	
	UCameraComponent* GetMainCamera() const { return MainCamera; }
	UMETWeaponManager* GetWeaponManager() const { return WeaponManager; }
	
	bool IsAiming() const { return bIsAiming; }
	bool IsTurningInPlace() const { return bIsTurningInPlace; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimDownSightsEvent, bool, bIsAiming);
	FAimDownSightsEvent& OnAimDownSights() { return AimDownSightsEvent; }

	FRotator GetActorControlRotationDelta() const { return ActorControlRotationDelta; }

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsAiming, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	bool bIsAiming;

	/** Called for movement input */
	void Move(const struct FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for ADS input */
	void AimingStarted();
	void AimingCompleted();

	/** Called for firing input */
	void FireActionStarted();
	void FireActionHeld();

	void Fire(bool bInHeld);

	UFUNCTION(Server, Reliable)
	void Server_Fire(bool bInHeld);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Fire(bool bInHeld);

	/* Aim down sights */
	void SetAiming(bool bInIsAiming);
	
	UFUNCTION(Server, Reliable)
	void Server_SetAiming(bool bInIsAiming);

	UFUNCTION()
	void OnRep_IsAiming();

	UFUNCTION()
	void WeaponManager_OnChangingWeaponsEvent(bool bInIsChangingWeapons);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(Replicated)
	FRotator RepControlRotation;
	
	FRotator ActorControlRotationDelta;
	
	FAimDownSightsEvent AimDownSightsEvent;

	UPROPERTY(Replicated)
	bool bIsTurningInPlace;

	void UpdateActorControlRotationDelta();
	bool IsActorControlRotationAligned() const;

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
