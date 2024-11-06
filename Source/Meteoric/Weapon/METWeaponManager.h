// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMETWeaponManager();

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class AMETWeapon* const InWeapon, int InSlot);

	UFUNCTION(BlueprintCallable)
	void UnequipCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	void CycleWeapon();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedEvent, AMETWeapon*, NewWeapon);
	FWeaponEquippedEvent& OnWeaponEquipped() { return WeaponEquippedEvent; }

	AMETWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AMETWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	int MaxWeapons;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient, VisibleAnywhere)
	TArray<TObjectPtr<AMETWeapon>> Weapons;
	
	int CurrentWeaponSlot;

	FWeaponEquippedEvent WeaponEquippedEvent;
};
