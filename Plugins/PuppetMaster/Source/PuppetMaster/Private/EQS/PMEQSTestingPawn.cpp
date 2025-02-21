// Copyright Alex Jobe


#include "EQS/PMEQSTestingPawn.h"

#include "Components/PMPuppetComponent.h"


APMEQSTestingPawn::APMEQSTestingPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	PuppetComponent = CreateDefaultSubobject<UPMPuppetComponent>("PuppetComponent");
	check(PuppetComponent)
}
