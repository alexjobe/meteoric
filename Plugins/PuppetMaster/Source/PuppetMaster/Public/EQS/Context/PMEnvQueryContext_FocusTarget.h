// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "PMEnvQueryContext_FocusTarget.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew, MinimalAPI)
class UPMEnvQueryContext_FocusTarget : public UEnvQueryContext
{
	GENERATED_BODY()

	PUPPETMASTER_API virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
	PUPPETMASTER_API static void LogError(const FString& Message, const FEnvQueryInstance& QueryInstance);
};
