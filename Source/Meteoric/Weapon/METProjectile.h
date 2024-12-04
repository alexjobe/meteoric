// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "METProjectile.generated.h"

UCLASS()
class METEORIC_API AMETProjectile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> CollisionComponent;
	
public:	
	AMETProjectile();

	void Fire(const FVector& InDirection) const;

	FGameplayEffectSpecHandle DamageEffectHandle;
};
