// Copyright Alex Jobe


#include "Tasks/PMBTTask_FireWeapon.h"

#include "AIController.h"
#include "Components/PMPuppetComponent.h"
#include "Logging/PuppetMasterLog.h"

UPMBTTask_FireWeapon::UPMBTTask_FireWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TimeToFireFor(5.f)
{
	NodeName = "FireWeapon";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UPMBTTask_FireWeapon::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

uint16 UPMBTTask_FireWeapon::GetInstanceMemorySize() const
{
	return sizeof(FBTFireWeaponMemory);
}

EBTNodeResult::Type UPMBTTask_FireWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;    
	}

	if (FBTFireWeaponMemory* Memory = reinterpret_cast<FBTFireWeaponMemory*>(NodeMemory); ensure(Memory))
	{
		Memory->TimeStartedFire = GetWorld()->GetTimeSeconds();
		Memory->TimeToFireFor = TimeToFireFor;
	}

	return EBTNodeResult::InProgress;
}

void UPMBTTask_FireWeapon::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner())
	{
		return;
	}

	const FBTFireWeaponMemory* Memory = reinterpret_cast<FBTFireWeaponMemory*>(NodeMemory);
	if (!ensure(Memory))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float TimeSeconds = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = TimeSeconds - Memory->TimeStartedFire;

	if (ElapsedTime >= Memory->TimeToFireFor)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	UPMPuppetComponent* PuppetComponent = AIController ? AIController->FindComponentByClass<UPMPuppetComponent>() : nullptr;

	if (ensure(PuppetComponent))
	{
		PuppetComponent->ActivateAbilityByTag(FireAbilityTag, true);
	}
	else
	{
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_FireWeapon -- PuppetMaster component not found! Owner: %s"), *OwnerString);

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
}
