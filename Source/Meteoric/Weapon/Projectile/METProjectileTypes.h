// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "METProjectileTypes.generated.h"

USTRUCT()
struct FMETProjectileDamageHandle
{
	GENERATED_BODY()

	FGameplayEffectSpecHandle EffectHandle;
	bool bExplosive;
	float ExplosionRadius;
	bool bApplyRocketJumpImpulse;

	FMETProjectileDamageHandle()
		: EffectHandle(nullptr)
		, bExplosive(false)
		, ExplosionRadius(150.f)
		, bApplyRocketJumpImpulse(false)
	{}

	bool IsValid() const { return EffectHandle.IsValid(); }
};