// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "METWeaponAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UMETWeaponAnimInstance();

	//~ Begin UAnimInstance interface
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	//~ End UAnimInstance interface

protected:
	UPROPERTY(Transient)
	TObjectPtr<class AMETWeapon> Weapon;

	UPROPERTY(BlueprintReadOnly, Category = "Reload")
	bool bReloading;

	UPROPERTY(BlueprintReadOnly, Category = "Reload")
	FTransform AmmoWorldTransform;

private:
	UFUNCTION()
	void Weapon_OnReloadEvent(const bool bInReloading);
};
