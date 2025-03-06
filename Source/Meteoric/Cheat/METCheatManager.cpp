// Copyright Alex Jobe


#include "METCheatManager.h"

#include "Components/PMCoverUserComponent.h"

void UMETCheatManager::DrawCoverDebug(const bool bShouldDraw) const
{
	const APlayerController* PlayerController = GetPlayerController();
	const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;

	if (UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr)
	{
		CoverUserComponent->DrawCoverDebug(bShouldDraw);
	}
}
