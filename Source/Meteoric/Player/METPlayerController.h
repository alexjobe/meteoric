// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "METPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* InputMappingContext;
	
	virtual void BeginPlay() override;
	
};
