// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PMCoverInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPMCoverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PMCOVERSYSTEM_API IPMCoverInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "PMCoverInterface")
	class UPMCoverComponent* GetCoverComponent() const;
};
