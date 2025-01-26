// Copyright Alex Jobe


#include "METAICharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Meteoric/AI/METAIController.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"

AMETAICharacter::AMETAICharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMETAbilitySystemComponent>("AbilitySystemComponent");
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->SetIsReplicated(true);

		// TODO: Currently set to Full because of an engine bug where duration gameplay cues don't replicate correctly
		// https://forums.unrealengine.com/t/gameplay-cue-and-periodic-effect-issue/723075
		AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Full);
	}

	AttributeSet = CreateDefaultSubobject<UMETAttributeSet>("AttributeSet");
}

void AMETAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	if (HasAuthority())
	{
		AddCharacterAbilities();
	}
}

void AMETAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	AMETAIController* AIController = Cast<AMETAIController>(NewController);
	check(AIController);

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();

	if (BlackboardComponent && BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AIController->RunBehaviorTree(BehaviorTree);
	}
}

void AMETAICharacter::InitAbilityActorInfo()
{
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		if (HasAuthority())
		{
			InitializeDefaultAttributes();
		}
	}
}
