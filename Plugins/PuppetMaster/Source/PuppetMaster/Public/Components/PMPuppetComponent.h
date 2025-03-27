// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "PuppetMasterTypes.h"
#include "PMPuppetComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUPPETMASTER_API UPMPuppetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPMPuppetComponent();

	UFUNCTION(BlueprintCallable)
	void InitializePuppet(class AAIController* InController, const FGameplayTag& InState);

	UFUNCTION(BlueprintCallable)
	void SetState(const FGameplayTag& InState);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetState() const { return StateTag; }

	UFUNCTION(BlueprintCallable)
	virtual void SetTargetActor(AActor* const InActor);

	UFUNCTION(BlueprintCallable)
	virtual void ClearTargetActor();
	
	UFUNCTION(BlueprintCallable)
	AActor* GetTargetActor() const { return TargetActor; }

	UFUNCTION(BlueprintCallable)
	void SetIdealTargetDistance(float InMinIdealTargetDistance, float InMaxIdealTargetDistance);

	UFUNCTION(BlueprintCallable)
	virtual FGameplayAbilitySpecHandle ActivateAbilityByTag(const FGameplayTag& InTag, const EPMAbilityActivationPolicy& InActivationPolicy = EPMAbilityActivationPolicy::OnInputStarted);

	UFUNCTION(BlueprintCallable)
	virtual void FinishAbilityByTag(const FGameplayTag& InTag);

	UFUNCTION(BlueprintCallable)
	AAIController* GetController() const { return Controller; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "StateTree|Events")
	FGameplayTag TargetActorUpdatedEventTag;
	
	/*
	 * Blackboard Values
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
	FGameplayTag StateTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Blackboard")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	float MinIdealTargetDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	float MaxIdealTargetDistance;
	
	/*
	 * Blackboard Key Names
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName StateTagKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName TargetActorKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName MinIdealTargetDistanceKeyName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName MaxIdealTargetDistanceKeyName;

	UFUNCTION()
	virtual void PerceptionComponent_OnTargetPerceptionUpdated(AActor* InActor, struct FAIStimulus InStimulus);

	virtual void HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus);
	virtual void HandleSense_Hearing(AActor& InActor, const FAIStimulus& InStimulus);
	virtual void HandleSense_Damage(AActor& InActor, const FAIStimulus& InStimulus);
	virtual void HandleSense_Touch(AActor& InActor, const FAIStimulus& InStimulus);

	virtual void TargetActor_OnGameplayTagEvent(FGameplayTag InTag, int32 InCount);

private:
	UPROPERTY(Transient)
	TObjectPtr<AAIController> Controller;

	UPROPERTY(Transient)
	TObjectPtr<class UStateTreeAIComponent> StateTreeAIComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UBlackboardComponent> BlackboardComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UAIPerceptionComponent> PerceptionComponent;
};
