// Copyright Alex Jobe


#include "METAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UMETAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UMETAttributeSet::OnRep_Health(const FGameplayAttributeData& InOldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, Health, InOldHealth);
}
