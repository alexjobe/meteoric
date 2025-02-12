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
	
	//~ Begin AMETCharacter interface
	virtual FVector GetFocalPoint() const override;
	//~ End AMETCharacter interface

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Death")
	float CorpseLifeSpan;

	UPROPERTY(Transient)
	TObjectPtr<class AMETAIController> AIController;
	
	//~ Begin AMETCharacter interface
	virtual void InitAbilityActorInfo() override;
	virtual void Die() override;
	//~ End AMETCharacter interface
};
