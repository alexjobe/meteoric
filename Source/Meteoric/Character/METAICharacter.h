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

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	//~ End ACharacter interface

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;
	
	//~ Begin AMETCharacter interface
	virtual void InitAbilityActorInfo() override;
	//~ End AMETCharacter interface
};
