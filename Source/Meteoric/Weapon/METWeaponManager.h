// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponManager : public UActorComponent
{
	GENERATED_BODY()

	/** Next Weapon Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> NextWeaponAction;

public:	
	UMETWeaponManager();

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class AMETWeapon* const InWeapon, int InSlot);

	UFUNCTION(BlueprintCallable)
	void UnequipCurrentWeapon();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedEvent, AMETWeapon*, NewWeapon);
	FWeaponEquippedEvent& OnWeaponEquipped() { return WeaponEquippedEvent; }

	AMETWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AMETWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	int MaxWeapons;

	/** Called for cycle weapon input */
	void CycleWeaponInput(const struct FInputActionValue& Value);

	void CycleWeapon(bool bInNext);

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient, VisibleAnywhere)
	TArray<TObjectPtr<AMETWeapon>> Weapons;
	
	int CurrentWeaponSlot;

	FWeaponEquippedEvent WeaponEquippedEvent;

public:
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
};
