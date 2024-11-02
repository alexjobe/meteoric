// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponTypes.h"
#include "METRecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETRecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMETRecoilComponent();

	void OnWeaponEquipped(ACharacter* const InOwningCharacter, const EWeaponFiringMode& InFiringMode);
	void OnFireActionStarted();
	void OnFireActionHeld();
	void OnWeaponFired();

protected:
	/* Aim recoil pattern */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	UCurveFloat* AimRecoilCurve;

	/* Multiplier applied to vertical aim recoil direction after normalizing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float VerticalAimRecoilMultiplier;

	/* Multiplier applied to horizontal aim recoil direction after normalizing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float HorizontalAimRecoilMultiplier;

	/* Determines min/max random value added to recoil yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float AimRecoilNoise;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;
	
	TEnumAsByte<EWeaponFiringMode> FiringMode;

	float FireActionStartTime;
	FVector2d CurrentRecoilCurvePos;
	FVector2d LastRecoilCurvePos;
};
