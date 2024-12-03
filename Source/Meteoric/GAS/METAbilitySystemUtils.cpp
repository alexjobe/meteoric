// Copyright Alex Jobe


#include "METAbilitySystemUtils.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

FActiveGameplayEffectHandle UMETAbilitySystemUtils::ApplyEffectToActor(const AActor* InTarget, const AActor* InSource, const TSubclassOf<UGameplayEffect>& InEffectClass, float InLevel)
{
	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(InTarget);
	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;
	
	if (!ensure(AbilitySystemComponent) || !ensure(InEffectClass)) return FActiveGameplayEffectHandle();

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(InSource);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InEffectClass, InLevel, ContextHandle);
	return AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}

void UMETAbilitySystemUtils::RemoveEffectFromActor(const AActor* InTarget, const FActiveGameplayEffectHandle& InHandle, int32 StacksToRemove)
{
	const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(InTarget);
	UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface ? AbilitySystemInterface->GetAbilitySystemComponent() : nullptr;

	if (!ensure(AbilitySystemComponent)) return;

	AbilitySystemComponent->RemoveActiveGameplayEffect(InHandle, StacksToRemove);
	
}
