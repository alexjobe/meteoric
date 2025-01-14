// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponSpreadComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpreadConeSettings
{
	GENERATED_BODY()

	/* Minimum yaw angle of the spread cone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread", meta=(ClampMin="0", ClampMax="180"))
	float MinYawInDegrees;

	/* Minimum pitch angle of the spread cone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread", meta=(ClampMin="0", ClampMax="180"))
	float MinPitchInDegrees;

	/* Maximum yaw angle of the spread cone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread", meta=(ClampMin="0", ClampMax="180"))
	float MaxYawInDegrees;

	/* Maximum pitch angle of the spread cone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread", meta=(ClampMin="0", ClampMax="180"))
	float MaxPitchInDegrees;

	/* How much each shot increases current spread cone yaw per shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float YawIncreasePerShot;

	/* How much each shot increases current spread cone pitch per shot */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float PitchIncreasePerShot;

	FSpreadConeSettings()
		: MinYawInDegrees(2.f)
		, MinPitchInDegrees(2.f)
		, MaxYawInDegrees(5.f)
		, MaxPitchInDegrees(5.f)
		, YawIncreasePerShot(0.5f)
		, PitchIncreasePerShot(0.5f)
	{}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponSpreadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMETWeaponSpreadComponent();

	/* How far to trace from the camera when determining shot direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
	float AimTraceRange;

	/* How much time must elapse since last shot before spread cone starts lerping back to minimum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Spread")
	float SpreadCooldown;

	/* How fast spread cone lerps back to minimum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Spread")
	float SpreadResetLerpSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Spread|Default")
	FSpreadConeSettings DefaultConeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Spread|ADS")
	FSpreadConeSettings AimConeSettings;

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	//~ End UActorComponent interface

	void UpdateWeaponSpread(const float InDeltaTime);

	FTransform GetProjectileSpawnTransform() const;

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnWeaponUnequipped();

	void OnWeaponFired();
	void OnAimDownSights(const bool bInIsAiming);

protected:
	FVector GetShotDirection(const FVector& InStartLocation) const;
	void ResetCurrentCone() { CurrentYawInDegrees = CurrentConeSettings.MinYawInDegrees; CurrentPitchInDegrees = CurrentConeSettings.MinPitchInDegrees; }

	FSpreadConeSettings CurrentConeSettings;
	float CurrentYawInDegrees;
	float CurrentPitchInDegrees;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(Transient)
	TObjectPtr<class AMETCharacter> OwningCharacter;
	
	float ElapsedTimeSinceFired;

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	void DrawSpreadDebugCone() const;
#endif
};
