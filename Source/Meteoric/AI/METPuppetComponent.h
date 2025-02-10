// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/PMPuppetComponent.h"
#include "METPuppetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class METEORIC_API UMETPuppetComponent : public UPMPuppetComponent
{
	GENERATED_BODY()

public:
	UMETPuppetComponent();

protected:
	//~ Begin UPMPuppetComponent interface
	virtual void HandleSense_Sight(AActor& InActor, const FAIStimulus& InStimulus) override;
	virtual void HandleSense_Hearing(AActor& InActor, const FAIStimulus& InStimulus) override;
	//~ End UPMPuppetComponent interface
};
