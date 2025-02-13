// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "METInputConfig.h"
#include "METInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename StartedFuncType, typename CompletedFuncType, typename TriggeredFuncType>
	void BindAbilityActions(const UMETInputConfig* InputConfig, UserClass* Object, StartedFuncType InputStartedFunc, CompletedFuncType InputCompletedFunc, TriggeredFuncType InputTriggeredFunc);
	
};

template <class UserClass, typename StartedFuncType, typename CompletedFuncType, typename TriggeredFuncType>
void UMETInputComponent::BindAbilityActions(const UMETInputConfig* InputConfig, UserClass* Object, StartedFuncType InputStartedFunc, CompletedFuncType InputCompletedFunc, TriggeredFuncType InputTriggeredFunc)
{
	check(InputConfig);

	for (const FMETInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if(InputStartedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, InputStartedFunc, Action.InputTag);
			}
			if(InputCompletedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, InputCompletedFunc, Action.InputTag);
			}
			if(InputTriggeredFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, InputTriggeredFunc, Action.InputTag);
			}
		}
	}
}
