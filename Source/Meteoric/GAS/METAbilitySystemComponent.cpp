// Copyright Alex Jobe


#include "METAbilitySystemComponent.h"

#include "Abilities/METGameplayAbility.h"
#include "Meteoric/METLogChannels.h"

UMETAbilitySystemComponent::UMETAbilitySystemComponent()
	: bCharacterAbilitiesGiven(false)
{
}

void UMETAbilitySystemComponent::AddAbilities(const TArray<TSubclassOf<UGameplayAbility>>& InAbilities, UObject* InSourceObject)
{
	for(const auto& Ability : InAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1, INDEX_NONE, InSourceObject);
		if(const UMETGameplayAbility* MetAbility = Cast<UMETGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(MetAbility->GetInputTag());
			GiveAbility(AbilitySpec);
		}
	}
}

FGameplayAbilitySpecHandle UMETAbilitySystemComponent::Input_AbilityInputStarted(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		return ActivateAbility(InputTag, EMETAbilityActivationPolicy::OnInputStarted);
	}
	UE_LOG(LogMETAbilitySystem, Warning, TEXT("UMETAbilitySystemComponent::Input_AbilityInputStarted -- Invalid input tag!"))
	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpecHandle UMETAbilitySystemComponent::Input_AbilityInputTriggered(const FGameplayTag& InputTag)
{
	if(InputTag.IsValid())
	{
		return ActivateAbility(InputTag, EMETAbilityActivationPolicy::OnInputTriggered);
	}
	UE_LOG(LogMETAbilitySystem, Warning, TEXT("UMETAbilitySystemComponent::Input_AbilityInputTriggered -- Invalid input tag!"))
	return FGameplayAbilitySpecHandle();
}

FGameplayAbilitySpecHandle UMETAbilitySystemComponent::ActivateAbility(const FGameplayTag& InputTag, const EMETAbilityActivationPolicy& InActivationPolicy)
{
	FGameplayAbilitySpecHandle Handle;
	if(!InputTag.IsValid()) return Handle;

	bool bAbilityFound = false;
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			const UMETGameplayAbility* MetAbility = CastChecked<UMETGameplayAbility>(AbilitySpec.Ability);
			bAbilityFound = true;
			if (MetAbility->GetActivationPolicy() == InActivationPolicy)
			{
				AbilitySpecInputPressed(AbilitySpec);
				if (!AbilitySpec.IsActive())
				{
					TryActivateAbility(AbilitySpec.Handle);
				}
				Handle = AbilitySpec.Handle;
				break;
			}
		}
	}
	if(!bAbilityFound)
	{
		UE_LOG(LogMETAbilitySystem, Warning, TEXT("UMETAbilitySystemComponent::ActivateAbility -- Unable to find ability with input tag %s!"), *InputTag.ToString())
	}
	
	return Handle;
}

void UMETAbilitySystemComponent::Input_AbilityInputCompleted(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;

	bool bAbilityFound = false;
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.Ability && AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpec.InputPressed = false;
			if (AbilitySpec.IsActive())
			{
				AbilitySpecInputReleased(AbilitySpec);
			}
			bAbilityFound = true;
		}
	}
	if(!bAbilityFound)
	{
		UE_LOG(LogMETAbilitySystem, Warning, TEXT("UMETAbilitySystemComponent::Input_AbilityInputCompleted -- Unable to find ability with input tag %s"), *InputTag.ToString())
	}
}

void UMETAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// Copied from Lyra - needed for WaitInputPress ability task
	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UMETAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	
	// Copied from Lyra - needed for WaitInputPress ability task
	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}
