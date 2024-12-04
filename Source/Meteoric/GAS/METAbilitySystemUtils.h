// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "METAbilitySystemUtils.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETAbilitySystemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GAS")
	static FActiveGameplayEffectHandle ApplyEffectClassToActor(const AActor* InTarget, const AActor* InSource, const TSubclassOf<UGameplayEffect>& InEffectClass, float InLevel);

	UFUNCTION(BlueprintCallable, Category = "GAS")
	static void RemoveEffectFromActor(const AActor* InTarget, const FActiveGameplayEffectHandle& InHandle, int32 StacksToRemove=-1);
};
