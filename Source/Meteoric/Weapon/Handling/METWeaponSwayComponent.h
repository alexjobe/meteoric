// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponSwayComponent.generated.h"

USTRUCT(BlueprintType)
struct FMETSwaySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway", meta = (ClampMin = 0, ClampMax = 90))
	float MaxSwayRoll;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway", meta = (ClampMin = 0, ClampMax = 90))
	float MaxSwayYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway", meta = (ClampMin = 0, ClampMax = 90))
	float MaxSwayPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway", meta = (ClampMin = 0))
	float MaxSwayLateral;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway", meta = (ClampMin = 0))
	float MaxSwayVertical;

	FMETSwaySettings()
		: MaxSwayRoll(0.0f)
		, MaxSwayYaw(0.f)
		, MaxSwayPitch(0.f)
		, MaxSwayLateral(0.f)
		, MaxSwayVertical(0.f)
	{}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponSwayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	FMETSwaySettings DefaultSwaySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	FMETSwaySettings AimSwaySettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float RotationSwaySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float PositionSwaySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float RotationInterpToZeroSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Sway")
	float PositionInterpToZeroSpeed;
	
	UMETWeaponSwayComponent();
	
	void UpdateWeaponSway(const float InDeltaTime);

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnAimDownSights(const bool bInIsAiming);

	void Reset();
	
	FTransform GetWeaponSway() const { return CurrentWeaponSway; }

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;
	
	TOptional<FRotator> PreviousControlRotation;
	FTransform TargetWeaponSway;
	FTransform CurrentWeaponSway;

	bool bIsAiming;
};
