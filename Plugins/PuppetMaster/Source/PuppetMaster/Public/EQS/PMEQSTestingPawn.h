// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EQSTestingPawn.h"
#include "Interface/PuppetMasterInterface.h"
#include "PMEQSTestingPawn.generated.h"

UCLASS()
class PUPPETMASTER_API APMEQSTestingPawn : public AEQSTestingPawn, public IPuppetMasterInterface
{
	GENERATED_BODY()

public:
	APMEQSTestingPawn();

	//~ Begin IPuppetMasterInterface interface
	UFUNCTION(BlueprintCallable)
	virtual UPMPuppetComponent* GetPuppetComponent() const override { return PuppetComponent; };
	//~ End IPuppetMasterInterface interface

protected:
	// For use with compatible queries
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puppet")
	TObjectPtr<UPMPuppetComponent> PuppetComponent;
};
