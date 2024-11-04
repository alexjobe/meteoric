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
	void EquipWeapon(class AMETWeapon* const InWeapon);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedEvent, AMETWeapon*, NewWeapon);
	FWeaponEquippedEvent& OnWeaponEquipped() { return WeaponEquippedEvent; }

	AMETWeapon* GetCurrentWeapon() const { return CurrentWeapon; }

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AMETWeapon> CurrentWeapon;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	FWeaponEquippedEvent WeaponEquippedEvent;
};
