// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PMCoverSystemUtils.generated.h"

struct FBlackboardKeySelector;
class UBlackboardComponent;
/**
 * 
 */
UCLASS()
class PMCOVERSYSTEM_API UPMCoverSystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FVector GetKeyLocation(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key);
	static float GetKeyFloatValue(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key);
};
