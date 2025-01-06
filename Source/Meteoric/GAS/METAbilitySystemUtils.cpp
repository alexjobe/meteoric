// Copyright Alex Jobe


#include "METAbilitySystemUtils.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

FActiveGameplayEffectHandle UMETAbilitySystemUtils::ApplyEffectClassToActor(const AActor* InTarget, const AActor* InSource, const TSubclassOf<UGameplayEffect>& InEffectClass, float InLevel)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InTarget);
	
	if (!ensure(AbilitySystemComponent) || !ensure(InEffectClass)) return FActiveGameplayEffectHandle();

	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(InSource);
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InEffectClass, InLevel, ContextHandle);
	return AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
}

void UMETAbilitySystemUtils::RemoveEffectFromActor(const AActor* InTarget, const FActiveGameplayEffectHandle& InHandle, int32 StacksToRemove)
{
	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InTarget);

	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(InHandle, StacksToRemove);
	}
}

void UMETAbilitySystemUtils::AddHitResultToEffectSpec(const FGameplayEffectSpecHandle& InEffectSpecHandle, const FHitResult& InHitResult)
{
	if (const FGameplayEffectSpec* EffectHandle = InEffectSpecHandle.Data.Get())
	{
		FGameplayEffectContextHandle ContextHandle = EffectHandle->GetEffectContext();
		ContextHandle.AddHitResult(InHitResult);
	}
}