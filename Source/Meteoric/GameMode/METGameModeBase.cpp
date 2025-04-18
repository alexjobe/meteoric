// Copyright Alex Jobe


#include "METGameModeBase.h"

#include "GenericTeamAgentInterface.h"
#include "Teams/PMTeamSettings.h"


AMETGameModeBase::AMETGameModeBase()
	: RespawnDelay(5.f)
{
}

void AMETGameModeBase::StartPlay()
{
	FGenericTeamId::SetAttitudeSolver(&UPMTeamSettings::GetAttitude);
	Super::StartPlay();
}

void AMETGameModeBase::PlayerDied(AController* InController)
{
	FTimerHandle RespawnTimerHandle;
	const FTimerDelegate RespawnTimerDelegate = FTimerDelegate::CreateUObject(this, &ThisClass::RespawnPlayer, InController);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnTimerDelegate, RespawnDelay, false);
}

void AMETGameModeBase::RespawnPlayer(AController* InController)
{
	if (!ensure(InController) || !InController->IsPlayerController()) return;

	AActor* PlayerStart = FindPlayerStart(InController);
	if (!ensure(PlayerStart)) return;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);
	if (ensure(NewPawn))
	{
		APawn* OldPawn = InController->GetPawn();
		InController->UnPossess();
		InController->Possess(NewPawn);
		if (OldPawn)
		{
			OldPawn->Destroy();
		}
	}
}