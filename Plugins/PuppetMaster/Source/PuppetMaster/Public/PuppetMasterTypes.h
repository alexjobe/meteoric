﻿// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PuppetMasterTypes.generated.h"

UENUM(BlueprintType)
enum class EPMAbilityActivationPolicy : uint8
{
	OnInputStarted,
	OnInputTriggered
};