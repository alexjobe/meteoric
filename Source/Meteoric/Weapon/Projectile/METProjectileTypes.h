// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "METProjectileTypes.generated.h"

enum class EDamageTiming : uint8
{
	Impact,
	Delayed
};

USTRUCT()
struct FMETProjectileDamageHandle
{
	GENERATED_BODY()

	FGameplayEffectSpecHandle EffectHandle;
	bool bExplosive;
	float ExplosionRadius;
	bool bApplyRocketJumpImpulse;
	EDamageTiming DamageTiming;

	FMETProjectileDamageHandle()
		: EffectHandle(nullptr)
		, bExplosive(false)
		, ExplosionRadius(0.f)
		, bApplyRocketJumpImpulse(false)
		, DamageTiming(EDamageTiming::Impact)
	{}

	bool IsValid() const { return EffectHandle.IsValid(); }
};