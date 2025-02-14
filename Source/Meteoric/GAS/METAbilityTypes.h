// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class EMETAbilityActivationPolicy : uint8
{
	OnInputStarted,
	OnInputTriggered
};