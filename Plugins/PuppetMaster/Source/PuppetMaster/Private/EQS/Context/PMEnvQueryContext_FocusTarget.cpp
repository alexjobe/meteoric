// Copyright Alex Jobe


#include "EQS/Context/PMEnvQueryContext_FocusTarget.h"

#include "Components/PMPuppetComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Interface/PuppetMasterInterface.h"
#include "Logging/PuppetMasterLog.h"

void UPMEnvQueryContext_FocusTarget::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	const IPuppetMasterInterface* PuppetMasterInterface = Cast<IPuppetMasterInterface>(QueryInstance.Owner.Get());
	
	if (const UPMPuppetComponent* PuppetComponent = PuppetMasterInterface ? PuppetMasterInterface->GetPuppetComponent() : nullptr)
	{
		if (const AActor* FocusTarget = PuppetComponent->GetFocusTarget())
		{
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, FocusTarget);
		}
		else
		{
			LogError("UPMEnvQueryContext_FocusTarget::ProvideContext -- Owner has no FocusTarget!", QueryInstance);
		}
	}
	else
	{
		LogError("UPMEnvQueryContext_FocusTarget::ProvideContext -- Owner must implement IPuppetMasterInterface!", QueryInstance);
	}
}

void UPMEnvQueryContext_FocusTarget::LogError(const FString& Message, const FEnvQueryInstance& QueryInstance)
{
	const AActor* OwningActor = Cast<AActor>(QueryInstance.Owner.Get());
	const FString OwnerString = OwningActor ? OwningActor->GetName() : QueryInstance.QueryName;
	const FString LogMessage = Message + " Owner: " + OwnerString;
	
	UE_LOG(LogPuppetMaster, Error, TEXT("%s"), *LogMessage);
}
