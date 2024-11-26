// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "METPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UMETInputConfig> InputConfig;
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	void Input_AbilityInputTagHeld(FGameplayTag InputTag);

	class UMETAbilitySystemComponent* GetAbilitySystemComponent();

private:
	UPROPERTY(Transient)
	TObjectPtr<UMETAbilitySystemComponent> AbilitySystemComponent;
	
};
