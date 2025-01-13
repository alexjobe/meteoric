// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "METProjectileWeaponComponent.generated.h"

USTRUCT()
struct FMETSpawnProjectileParams
{
	GENERATED_BODY()
	
	FTransform SpawnTransform;

	UPROPERTY(Transient)
	AActor* Owner;

	UPROPERTY(Transient)
	APawn* Instigator;

	FGameplayEffectSpecHandle ImpactDamageEffectHandle;
	FGameplayEffectSpecHandle DelayedDamageEffectHandle;

	FMETSpawnProjectileParams()
		: Owner(nullptr)
		, Instigator(nullptr)
	{}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETProjectileWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AMETProjectile> ProjectileClass;

public:	
	UMETProjectileWeaponComponent();

	void FireProjectile(const FMETSpawnProjectileParams& Params) const;
};
