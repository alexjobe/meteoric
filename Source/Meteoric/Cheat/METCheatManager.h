// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "METCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	UFUNCTION(Exec)
	void DrawCoverDebug(const bool bShouldDraw) const;

	UFUNCTION(Exec)
	void InfiniteAmmo(const bool bInfiniteAmmo) const;

	UFUNCTION(Exec)
	void FillReserveAmmo() const;
};
