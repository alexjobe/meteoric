// Copyright Alex Jobe


#include "METAICharacter.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/AI/METAIController.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "Perception/AISense_Damage.h"

AMETAICharacter::AMETAICharacter()
	: CorpseLifeSpan(10.f)
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

	if (ensure(AmmoManager))
	{
		AmmoManager->OnWeaponAmmoChanged().AddUniqueDynamic(this, &AMETAICharacter::AmmoManager_OnWeaponAmmoChanged);
	}
}

void AMETAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority()) return;

	AIController = Cast<AMETAIController>(NewController);
	check(AIController);

	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();

	if (BlackboardComponent && BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AIController->RunBehaviorTree(BehaviorTree);
	}
	else if (UStateTreeAIComponent* StateTreeAIComponent = AIController->GetStateTreeAIComponent())
	{
		AIController->BrainComponent = StateTreeAIComponent;
		StateTreeAIComponent->StartLogic();
	}
}

FVector AMETAICharacter::GetFocalPoint() const
{
	if (ensure(AIController))
	{
		return AIController->GetFocalPoint();
	}
	return GetActorLocation();
}

UPMPuppetComponent* AMETAICharacter::GetPuppetComponent() const
{
	if (ensure(AIController))
	{
		return AIController->GetPuppetComponent();
	}
	return nullptr;
}

void AMETAICharacter::InitAbilityActorInfo()
{
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		if (HasAuthority())
		{
			InitializeDefaultAttributes();
			BindAttributeChangedCallbacks();
			AbilitySystemComponent->SetTagMapCount(METGameplayTags::State_Dead, 0);
		}
	}
}

void AMETAICharacter::Die()
{
	if (UBrainComponent* BrainComponent = AIController ? AIController->GetBrainComponent() : nullptr; ensure(BrainComponent))
	{
		BrainComponent->StopLogic(FString("Dead"));
	}
	
	if(UCrowdFollowingComponent* CrowdFollowingComponent = AIController ? AIController->GetComponentByClass<UCrowdFollowingComponent>() : nullptr; ensure(CrowdFollowingComponent))
	{
		CrowdFollowingComponent->SetCrowdSimulationState(ECrowdSimulationState::Disabled);
	}
	
	Super::Die();
	
	SetLifeSpan(CorpseLifeSpan);
}

void AMETAICharacter::AttributeSet_OnAttributeDamageEvent(const float DamageAmount, const FGameplayEffectSpec& EffectSpec)
{
	Super::AttributeSet_OnAttributeDamageEvent(DamageAmount, EffectSpec);
	
	if (!ensure(AbilitySystemComponent) || AbilitySystemComponent->HasMatchingGameplayTag(METGameplayTags::State_Dead)) return;

	if (AActor* SourceActor = EffectSpec.GetContext().GetEffectCauser())
	{
		UAISense_Damage::ReportDamageEvent(this, this, SourceActor, DamageAmount, SourceActor->GetActorLocation(), GetActorLocation());
	}
}

void AMETAICharacter::AmmoManager_OnWeaponAmmoChanged(UMETAmmoDataAsset* const AmmoType, int32 AmmoCount, int32 MaxAmmo)
{
	if (AmmoCount < 1)
	{
		if (UStateTreeAIComponent* StateTreeAIComponent = AIController ? AIController->GetStateTreeAIComponent() : nullptr)
		{
			StateTreeAIComponent->SendStateTreeEvent(FStateTreeEvent(METGameplayTags::AIEvent_WeaponAmmoEmpty));
		}
	}
}
