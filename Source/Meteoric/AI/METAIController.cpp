// Copyright Alex Jobe


#include "METAIController.h"

#include "Components/PMPuppetComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

AMETAIController::AMETAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard)
	
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BrainComponent)

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	check(PerceptionComponent)

	PuppetComponent = CreateDefaultSubobject<UPMPuppetComponent>("PuppetComponent");
	check(PerceptionComponent)
}

void AMETAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(PuppetComponent))
	{
		PuppetComponent->InitializePuppet(this);
	}
}
