// Copyright Alex Jobe


#include "METPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Meteoric/Input/METInputComponent.h"

void AMETPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AMETPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UMETInputComponent* MetInputComponent = CastChecked<UMETInputComponent>(InputComponent);

	MetInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, &ThisClass::Input_AbilityInputTagHeld);
}

void AMETPlayerController::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("Input_AbilityInputTagPressed: %s"), *InputTag.ToString());
}

void AMETPlayerController::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("Input_AbilityInputTagReleased: %s"), *InputTag.ToString());
}

void AMETPlayerController::Input_AbilityInputTagHeld(FGameplayTag InputTag)
{
	UE_LOG(LogTemp, Warning, TEXT("Input_AbilityInputTagHeld: %s"), *InputTag.ToString());
}
