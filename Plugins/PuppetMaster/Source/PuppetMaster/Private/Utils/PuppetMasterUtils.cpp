// Copyright Alex Jobe


#include "Utils/PuppetMasterUtils.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "Logging/PuppetMasterLog.h"

bool UPuppetMasterUtils::IsAbilityActive(const FGameplayAbilitySpecHandle& InHandle, const AAIController& InController)
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
		UE_LOG(LogPuppetMaster, Error, TEXT("UPuppetMasterUtils::IsAbilityActive -- AbilitySystemComponent not found! Owner: %s"), *OwnerString);
	}
	
	return false;
}
