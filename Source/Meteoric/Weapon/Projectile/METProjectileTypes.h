// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METProjectileTypes.generated.h"

UENUM()
enum class EMETDamageTiming : uint8
{
	Impact,
	Delayed
};

USTRUCT(BlueprintType)
struct FMETProjectileFXSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TMap<TEnumAsByte<EPhysicalSurface>, TObjectPtr<UParticleSystem>> SurfaceToVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|FX")
	TMap<TEnumAsByte<EPhysicalSurface>, TObjectPtr<USoundCue>> SurfaceToSFX;
};