// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "METPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UMETInputConfig> InputConfig;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void Input_AbilityInputStarted(FGameplayTag InputTag);
	void Input_AbilityInputCompleted(FGameplayTag InputTag);
	void Input_AbilityInputTriggered(FGameplayTag InputTag);

	class UMETAbilitySystemComponent* GetAbilitySystemComponent();

	//~ Begin UGenericTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End UGenericTeamAgentInterface interface
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	FGameplayTag TeamTag;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMETAbilitySystemComponent> AbilitySystemComponent;
};
