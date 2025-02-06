// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "METAIController.generated.h"

UCLASS()
class METEORIC_API AMETAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMETAIController();

	virtual void BeginPlay() override;

	//~ Begin UGenericTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End UGenericTeamAgentInterface interface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puppet")
	TObjectPtr<class UPMPuppetComponent> PuppetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	FGameplayTag TeamTag;
};
