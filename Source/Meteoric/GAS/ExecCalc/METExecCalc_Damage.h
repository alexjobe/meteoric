// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "METExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UMETExecCalc_Damage();

	//~ Begin UGameplayEffectExecutionCalculation interface
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	//~ End UGameplayEffectExecutionCalculation interface
	
};
