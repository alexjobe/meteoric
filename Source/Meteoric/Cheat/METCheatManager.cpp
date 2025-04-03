// Copyright Alex Jobe


#include "METCheatManager.h"

#include "Components/PMCoverUserComponent.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"

void UMETCheatManager::DrawCoverDebug(const bool bShouldDraw) const
{
	const APlayerController* PlayerController = GetPlayerController();
	const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;

	if (UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr)
	{
		CoverUserComponent->DrawCoverDebug(bShouldDraw);
	}
}

void UMETCheatManager::InfiniteAmmo(const bool bInfiniteAmmo) const
{
	const APlayerController* PlayerController = GetPlayerController();
	const APawn* Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	if (UMETAmmoManager* AmmoManager = Pawn ? Pawn->FindComponentByClass<UMETAmmoManager>() : nullptr)
	{
		AmmoManager->bInfiniteAmmo = bInfiniteAmmo;
	}
}
