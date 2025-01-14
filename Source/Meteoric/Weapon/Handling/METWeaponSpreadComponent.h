// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponSpreadComponent.generated.h"


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float SpreadCooldown;

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

	/* How fast spread cone lerps back to minimum */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float SpreadResetLerpSpeed;

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	//~ End UActorComponent interface

	void UpdateWeaponSpread(const float InDeltaTime);

	FTransform GetProjectileSpawnTransform() const;

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnWeaponUnequipped();

	void OnWeaponFired();

protected:
	FVector GetShotDirection(const FVector& InStartLocation) const;

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
