// Copyright Alex Jobe


#include "METCheatManager.h"

#include "Components/PMCoverUserComponent.h"
#include "Meteoric/Character/METCharacter.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"
#include "Meteoric/Weapon/Ammo/METWeaponAmmoComponent.h"

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

void UMETCheatManager::FillReserveAmmo() const
{
	const APlayerController* PlayerController = GetPlayerController();
	const AMETCharacter* Character = PlayerController ? Cast<AMETCharacter>(PlayerController->GetPawn()) : nullptr;
	const AMETWeapon* Weapon = Character ? Character->GetWeapon() : nullptr;
	if (UMETWeaponAmmoComponent* AmmoComponent = Weapon ? Weapon->GetAmmoComponent() : nullptr)
	{
		AmmoComponent->FillReserveAmmo();
	}
}
