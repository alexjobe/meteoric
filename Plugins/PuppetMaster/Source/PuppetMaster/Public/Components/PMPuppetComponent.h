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

	UFUNCTION(BlueprintCallable)
	void InitializePuppet(class AAIController* InController);

	UFUNCTION(BlueprintCallable)
	AActor* GetFocusTarget() const { return FocusTarget; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusTarget;

	UFUNCTION()
	void PerceptionComponent_OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

private:
	UPROPERTY(Transient)
	TObjectPtr<AAIController> AIController;

	UPROPERTY(Transient)
	TObjectPtr<class UAIPerceptionComponent> PerceptionComponent;
	
};
