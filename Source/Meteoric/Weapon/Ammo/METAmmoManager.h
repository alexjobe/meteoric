// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METAmmoManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETAmmoManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UMETAmmoManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxCount;

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 GetAmmoCount(class UMETAmmoDataAsset* const InType);

	// Adds ammo and returns new count
	UFUNCTION(BlueprintCallable, Category = "Ammo")
	int32 AddAmmo(UMETAmmoDataAsset* const InType, const int32 InAmmoCount);

	// Returns amount successfully consumed
	int32 TryConsumeAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount = 1);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TMap<TObjectPtr<UMETAmmoDataAsset>, int32> AmmoTypeToCount;
};
