// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponSwayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponSwayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRateRoll_Default;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRateYaw_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRatePitch_Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRateRoll_Aim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRateYaw_Aim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float SwayRatePitch_Aim;
	
	UMETWeaponSwayComponent();
	
	void UpdateWeaponSway(float InDeltaTime);

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnAimDownSights(bool bInIsAiming);

	FRotator GetWeaponSway() const { return CurrentWeaponSway; }

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;
	
	TOptional<FRotator> PreviousControlRotation;
	FRotator TargetWeaponSway;
	FRotator CurrentWeaponSway;

	bool bIsAiming;
};
