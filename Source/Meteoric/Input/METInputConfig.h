// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "METInputConfig.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMETInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	FGameplayTag InputTag;

	FMETInputAction()
		: InputAction(nullptr)
	{}
};

UCLASS(BlueprintType, Const)
class METEORIC_API UMETInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TArray<FMETInputAction> AbilityInputActions;
};
