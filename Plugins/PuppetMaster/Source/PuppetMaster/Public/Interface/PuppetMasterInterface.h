// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PuppetMasterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UPuppetMasterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PUPPETMASTER_API IPuppetMasterInterface
{
	GENERATED_BODY()

public:
	/** Returns the puppet component to use for this actor. It may live on another actor, such as a Pawn using the Controller's component */
	virtual class UPMPuppetComponent* GetPuppetComponent() const = 0;
};
