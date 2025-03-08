// Copyright Alex Jobe


#include "Tasks/PMBTTask_ActivateAbility_Latent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "Components/PMPuppetComponent.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"

UPMBTTask_ActivateAbility_Latent::UPMBTTask_ActivateAbility_Latent()
	: ActivationPolicy(EPMAbilityActivationPolicy::OnInputStarted)
	, CompletionPolicy(EPMTaskNodeCompletionPolicy::Duration)
	, Duration(5.f)
{
	NodeName = "ActivateAbility_Latent";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

uint16 UPMBTTask_ActivateAbility_Latent::GetInstanceMemorySize() const
{
	return sizeof(FBTActivateAbilityMemory);
}

EBTNodeResult::Type UPMBTTask_ActivateAbility_Latent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!OwnerComp.GetAIOwner())
	{
		return EBTNodeResult::Failed;    
	}

	if (FBTActivateAbilityMemory* Memory = reinterpret_cast<FBTActivateAbilityMemory*>(NodeMemory); ensure(Memory))
	{
		Memory->TimeStarted = GetWorld()->GetTimeSeconds();
		Memory->Duration = Duration;
		Memory->bAbilityStarted = false;
		Memory->AbilitySpecHandle = FGameplayAbilitySpecHandle();
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UPMBTTask_ActivateAbility_Latent::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(OwnerComp.GetAIOwner());
	if (UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr; ensure(PuppetComponent))
	{
		PuppetComponent->FinishAbilityByTag(AbilityTag);
	}
	else
	{
		const AAIController* AIController = OwnerComp.GetAIOwner();
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Latent::AbortTask -- Owner must implement IPuppetMasterInterface! Owner: %s"), *OwnerString);
	}
	
	return EBTNodeResult::Aborted;
}

void UPMBTTask_ActivateAbility_Latent::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!OwnerComp.GetAIOwner())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FBTActivateAbilityMemory* Memory = reinterpret_cast<FBTActivateAbilityMemory*>(NodeMemory);
	if (!ensure(Memory))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const AAIController* AIController = OwnerComp.GetAIOwner();
	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(AIController);
	
	UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr;
	if (!ensure(PuppetComponent))
	{
		const FString OwnerString = AIController ? AIController->GetName() : OwnerComp.GetName();
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Latent::TickTask -- Owner must implement IPuppetMasterInterface! Owner: %s"), *OwnerString);

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float TimeSeconds = GetWorld()->GetTimeSeconds();
	const float ElapsedTime = TimeSeconds - Memory->TimeStarted;

	if (CompletionPolicy == EPMTaskNodeCompletionPolicy::Duration && ElapsedTime >= Memory->Duration)
	{
		if (Memory->bAbilityStarted)
		{
			PuppetComponent->FinishAbilityByTag(AbilityTag);
		}
		else
		{
			const FString OwnerString = AIController->GetName();;
			UE_LOG(LogPuppetMaster, Warning, TEXT(
				"UPMBTTask_ActivateAbility_Latent::TickTask -- Task node finished before ability started! "
				"Make sure duration is long enough. Owner: %s"),
				*OwnerString);
		}
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (CompletionPolicy == EPMTaskNodeCompletionPolicy::OnAbilityEnd && Memory->bAbilityStarted)
	{
		if (!ensure(Memory->AbilitySpecHandle.IsValid()))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
			return;
		}
		
		if (!IsAbilityActive(Memory->AbilitySpecHandle, *AIController))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
	
	if (ActivationPolicy == EPMAbilityActivationPolicy::OnInputTriggered || !Memory->bAbilityStarted)
	{
		Memory->AbilitySpecHandle = PuppetComponent->ActivateAbilityByTag(AbilityTag, ActivationPolicy);
		Memory->bAbilityStarted = true;
	}
}

void UPMBTTask_ActivateAbility_Latent::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTActivateAbilityMemory>(NodeMemory, InitType);
}

void UPMBTTask_ActivateAbility_Latent::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTActivateAbilityMemory>(NodeMemory, CleanupType);
}

bool UPMBTTask_ActivateAbility_Latent::IsAbilityActive(const FGameplayAbilitySpecHandle& InHandle, const AAIController& InController)
{
	const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(&InController);
	if (!ASC)
	{
		ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InController.GetPawn());
	}
	
	if (ensure(ASC))
	{
		if (const FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromHandle(InHandle))
		{
			if (AbilitySpec->IsActive())
			{
				return true;
			}
		}
	}
	else
	{
		const FString OwnerString = InController.GetName();;
		UE_LOG(LogPuppetMaster, Error, TEXT("UPMBTTask_ActivateAbility_Latent::IsAbilityActive -- AbilitySystemComponent not found! Owner: %s"), *OwnerString);
	}
	
	return false;
}
