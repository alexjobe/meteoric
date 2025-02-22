// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METCharacterTypes.generated.h"

USTRUCT(BlueprintType)
struct FMETCharacterConfig
{
	GENERATED_BODY()

	// How fast aim rotation interpolates to control rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float AimInterpSpeed;

	// How fast to turn in place when standing still
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float TurnInPlaceSpeed_Stationary;

	// How fast to turn in place when moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
	float TurnInPlaceSpeed_Moving;

	FMETCharacterConfig()
		: AimInterpSpeed(20.0f)
		, TurnInPlaceSpeed_Stationary(10.f)
		, TurnInPlaceSpeed_Moving(20.f)
	{}
};