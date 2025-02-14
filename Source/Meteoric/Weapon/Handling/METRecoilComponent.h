// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Meteoric/Math/METSpring.h"
#include "Meteoric/Weapon/METWeaponTypes.h"
#include "METRecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETRecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMETRecoilComponent();

	virtual void BeginPlay() override;

	void UpdateRecoil(float InDeltaTime);

	void OnWeaponEquipped(ACharacter* const InOwningCharacter, const EWeaponFiringMode& InFiringMode);
	void OnFireActionHeld();
	void OnWeaponFired();

	void Reset();

	FTransform GetSpringRecoilTransform() const { return SpringRecoilTransform; }

protected:
	/* Aim recoil pattern */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Aim")
	UCurveFloat* AimRecoilCurve;

	/* Multiplier applied to vertical aim recoil direction after normalizing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil|Aim")
	float VerticalAimRecoilMultiplier;

	/* Multiplier applied to horizontal aim recoil direction after normalizing */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil|Aim")
	float HorizontalAimRecoilMultiplier;

	/* Determines min/max random value added to recoil yaw */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil|Aim")
	float AimRecoilNoise;
	
	/* Spring recoil */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	FMETSpring RecoilSpring_Z;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	FMETSpring RecoilSpring_Y;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	FMETSpring RecoilSpring_Pitch;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	float RecoilForce_Z;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	float RecoilForce_Y;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil|Spring")
	float RecoilForce_Pitch;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;
	EWeaponFiringMode FiringMode;

	TOptional<FDelegateHandle> GameplayTagEventHandle_FireWeapon;

	/* Aim recoil */
	float FireActionStartTime;
	FVector2d CurrentRecoilCurvePos;
	FVector2d LastRecoilCurvePos;

	FTransform SpringRecoilTransform;
};
