// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Interface/PuppetMasterInterface.h"
#include "METAIController.generated.h"

UCLASS()
class METEORIC_API AMETAIController : public AAIController, public IPuppetMasterInterface
{
	GENERATED_BODY()

public:
	AMETAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	//~ Begin IGenericTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End IGenericTeamAgentInterface interface

	//~ Begin IPuppetMasterInterface interface
	UFUNCTION(BlueprintCallable)
	virtual UPMPuppetComponent* GetPuppetComponent() const override;
	//~ End IPuppetMasterInterface interface

	class UStateTreeAIComponent* GetStateTreeAIComponent() const { return StateTreeAIComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puppet")
	TObjectPtr<class UMETPuppetComponent> PuppetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "StateTreeAI", Category = "StateTree")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	FGameplayTag TeamTag;
};
