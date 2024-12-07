// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "METGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMETGameModeBase();
	
	void PlayerDied(AController* InController);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Respawn", meta = (AllowPrivateAccess = "true"))
	float RespawnDelay;

	void RespawnPlayer(AController* InController);
};
