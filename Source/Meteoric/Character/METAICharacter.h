// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METCharacter.h"
#include "METAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETAICharacter : public AMETCharacter
{
	GENERATED_BODY()

public:
	AMETAICharacter();

	//~ Begin AMETCharacter interface
	virtual void BeginPlay() override;
	//~ End AMETCharacter interface

protected:
	//~ Begin AMETCharacter interface
	virtual void InitAbilityActorInfo() override;
	//~ End AMETCharacter interface
};
