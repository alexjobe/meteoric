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

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class AMETWeapon* const InWeapon);

	AMETWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	UCameraComponent* GetMainCamera() const { return MainCamera; }
	bool IsAiming() const { return bIsAiming; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedEvent, const AMETWeapon*, NewWeapon);
	FWeaponEquippedEvent& OnWeaponEquipped() { return WeaponEquippedEvent; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimDownSightsEvent, bool, bIsAiming);
	FAimDownSightsEvent& OnAimDownSights() { return AimDownSightsEvent; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AMETWeapon> CurrentWeapon;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	bool bIsAiming;
	
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const struct FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for ADS input */
	void AimDownSightsStarted();
	void AimDownSightsCompleted();

	/** Called for firing input */
	void FireStarted();
	void FireHeld();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FWeaponEquippedEvent WeaponEquippedEvent;
	FAimDownSightsEvent AimDownSightsEvent;

};