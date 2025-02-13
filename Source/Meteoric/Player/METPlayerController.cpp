// Copyright Alex Jobe


#include "METPlayerController.h"

#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/Input/METInputComponent.h"
#include "Teams/PMTeamSettings.h"

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

	MetInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputStarted, &ThisClass::Input_AbilityInputCompleted, &ThisClass::Input_AbilityInputTriggered);
}

void AMETPlayerController::Input_AbilityInputStarted(FGameplayTag InputTag)
{
	if (UMETAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->Input_AbilityInputStarted(InputTag);
	}
}

void AMETPlayerController::Input_AbilityInputCompleted(FGameplayTag InputTag)
{
	if (UMETAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->Input_AbilityInputCompleted(InputTag);
	}
}

void AMETPlayerController::Input_AbilityInputTriggered(FGameplayTag InputTag)
{
	if (UMETAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->Input_AbilityInputTriggered(InputTag);
	}
}

UMETAbilitySystemComponent* AMETPlayerController::GetAbilitySystemComponent()
{
	if (AbilitySystemComponent == nullptr)
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn());
		AbilitySystemComponent = CastChecked<UMETAbilitySystemComponent>(ASC);
	}
	return AbilitySystemComponent;
}

void AMETPlayerController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	if (InTeamID != GetGenericTeamId())
	{
		TeamTag = UPMTeamSettings::GetTeamTag(InTeamID);
	}
}

FGenericTeamId AMETPlayerController::GetGenericTeamId() const
{
	return UPMTeamSettings::GetTeamId(TeamTag);
}
