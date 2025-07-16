// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "METProjectileTypes.h"
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

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float LifeSpan;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DamageDelay;

	FMETProjectileDamageHandle ImpactDamageHandle;
	FMETProjectileDamageHandle DelayedDamageHandle;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void CollisionComponent_OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void Explode(const FMETProjectileDamageHandle& InDamage) const;

private:
	FTimerHandle DamageTimerHandle;

	static void ApplyDamageEffect(const AActor& InActor, const FGameplayEffectSpecHandle& InEffectHandle);
	
	void StartDelayedDamageTimer(AActor* InOtherActor, UPrimitiveComponent* InOtherComponent, const FHitResult& InHitResult);

	UFUNCTION()
	void ApplyDelayedDamage(AActor* InOtherActor);

	bool bCollided;
};