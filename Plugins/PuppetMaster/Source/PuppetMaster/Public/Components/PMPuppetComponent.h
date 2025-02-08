// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PMPuppetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PUPPETMASTER_API UPMPuppetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPMPuppetComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blackboard")
	FName FocusTargetKeyName;

	UFUNCTION(BlueprintCallable)
	void InitializePuppet(class AAIController* InController);

	UFUNCTION(BlueprintCallable)
	AActor* GetFocusTarget() const { return FocusTarget; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusTarget;

	UFUNCTION()
	virtual void PerceptionComponent_OnTargetPerceptionUpdated(AActor* InActor, struct FAIStimulus InStimulus);

	virtual void HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus);

private:
	UPROPERTY(Transient)
	TObjectPtr<AAIController> AIController;

	UPROPERTY(Transient)
	TObjectPtr<class UBlackboardComponent> BlackboardComponent;

	UPROPERTY(Transient)
	TObjectPtr<class UAIPerceptionComponent> PerceptionComponent;
	
};
