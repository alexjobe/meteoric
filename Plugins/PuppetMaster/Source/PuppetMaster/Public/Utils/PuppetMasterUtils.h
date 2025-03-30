// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PuppetMasterUtils.generated.h"

/**
 * 
 */
UCLASS()
class PUPPETMASTER_API UPuppetMasterUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool IsAbilityActive(const struct FGameplayAbilitySpecHandle& InHandle, const class AAIController& InController);
};
