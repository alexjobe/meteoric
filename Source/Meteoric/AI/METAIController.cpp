// Copyright Alex Jobe


#include "METAIController.h"

#include "METPuppetComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Meteoric/METGameplayTags.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Teams/PMTeamSettings.h"

AMETAIController::AMETAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard)
	
	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BrainComponent)

	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComponent");
	check(PerceptionComponent)

	PuppetComponent = CreateDefaultSubobject<UMETPuppetComponent>("PuppetComponent");
	check(PuppetComponent)
}

void AMETAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(PuppetComponent))
	{
		PuppetComponent->InitializePuppet(this, METGameplayTags::AIState_Idle);
	}

	if (UCrowdFollowingComponent* CrowdFollowingComponent = GetComponentByClass<UCrowdFollowingComponent>(); ensure(CrowdFollowingComponent))
	{
		CrowdFollowingComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
	}
}

void AMETAIController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	if (InTeamID != GetGenericTeamId())
	{
		TeamTag = UPMTeamSettings::GetTeamTag(InTeamID);
	}
}

FGenericTeamId AMETAIController::GetGenericTeamId() const
{
	return UPMTeamSettings::GetTeamId(TeamTag);
}

UPMPuppetComponent* AMETAIController::GetPuppetComponent() const
{
	return PuppetComponent;
}
