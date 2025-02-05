// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "METAIController.generated.h"

UCLASS()
class METEORIC_API AMETAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMETAIController();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puppet")
	TObjectPtr<class UPMPuppetComponent> PuppetComponent;
};
