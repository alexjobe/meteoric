// Copyright Alex Jobe


#include "Tasks/PMBTTask_ActivateAbility_Held.h"

#include "AIController.h"
#include "Components/PMPuppetComponent.h"
#include "Logging/PuppetMasterLog.h"

UPMBTTask_ActivateAbility_Held::UPMBTTask_ActivateAbility_Held(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Duration(5.f)
	, ActivationPolicy(EPMAbilityActivationPolicy::OnInputStarted)
{
	NodeName = "ActivateAbility_Held";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UPMBTTask_ActivateAbility_Held::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

uint16 UPMBTTask_ActivateAbility_Held::GetInstanceMemorySize() const
{
	return sizeof(FBTActivateAbilityMemory);
}

EBTNodeResult::Type UPMBTTask_ActivateAbility_Held::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;    
	}

	if (FBTActivateAbilityMemory* Memory = reinterpret_cast<FBTActivateAbilityMemory*>(NodeMemory); ensure(Memory))
	{
		Memory->TimeStarted = GetWorld()->GetTimeSeconds();
		Memory->Duration = Duration;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UPMBTTask_ActivateAbility_Held::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	UPMPuppetComponent* PuppetComponent = AIController ? AIController->FindComponentByClass<UPMPuppetComponent>() : nullptr;
	if (ensure(PuppetComponent))
	{
		PuppetComponent->FinishAbilityByTag(AbilityTag);
	}
	else
	{
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Held::AbortTask -- PuppetComponent not found! Owner: %s"), *OwnerString);
	}
	
	return EBTNodeResult::Aborted;
}

void UPMBTTask_ActivateAbility_Held::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner())
	{
		return;
	}

	const FBTActivateAbilityMemory* Memory = reinterpret_cast<FBTActivateAbilityMemory*>(NodeMemory);
	if (!ensure(Memory))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	UPMPuppetComponent* PuppetComponent = AIController ? AIController->FindComponentByClass<UPMPuppetComponent>() : nullptr;
	if (!ensure(PuppetComponent))
	{
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Held::TickTask -- PuppetComponent not found! Owner: %s"), *OwnerString);

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float TimeSeconds = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = TimeSeconds - Memory->TimeStarted;

	if (ElapsedTime >= Memory->Duration)
	{
		PuppetComponent->FinishAbilityByTag(AbilityTag);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	PuppetComponent->ActivateAbilityByTag(AbilityTag, ActivationPolicy);
}
