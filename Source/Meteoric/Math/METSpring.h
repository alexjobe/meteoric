// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METSpring.generated.h"

USTRUCT(BlueprintType)
struct FMETSpring
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	float SpringConstant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	float DampingRatio;

	FMETSpring();

	void ApplyInstantaneousForce(float InForce);

	float UpdateSpring(float InDeltaTime);

private:
	float DampedAngularFrequency;
	float InitialVelocity;
	float CurrentVelocity;
	float InitialDisplacement;
	float CurrentDisplacement;
	float ElapsedTime;
};
