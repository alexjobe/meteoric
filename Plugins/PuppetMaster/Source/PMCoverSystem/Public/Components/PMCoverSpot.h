// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/SphereComponent.h"
#include "PMCoverSpot.generated.h"

/*
 * Represents a single cover spot that can be claimed by actors with a PMCoverUserComponent. Intended to be added to
 * actors with a PMCoverComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PMCOVERSYSTEM_API UPMCoverSpot : public USphereComponent
{
	GENERATED_BODY()

public:
	UPMCoverSpot();

	/*
	 * Cover score is the dot product between the cover spot's forward vector and the direction vector from the cover
	 * spot to the target location
	 */
	void InitializeCoverSpot(const TSubclassOf<class UGameplayEffect>& InCoverEffectClass, float InCoverEffectLevel);
	float GetCoverScore(const FVector& InTargetLocation) const;
	bool IsOccupied() const { return CurrentOccupant != nullptr; }
	bool ClaimCoverSpot(AActor* InActor);
	void ReleaseCoverSpot();
	AActor* GetCurrentOccupant() const { return CurrentOccupant; }

private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> CurrentOccupant;
	
	/* Effect applied to actor using this cover */
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> CoverEffectClass;

	/* CoverEffectClass level */
	float CoverEffectLevel;

	TOptional<FActiveGameplayEffectHandle> ActiveCoverEffectHandle;

	void ApplyCoverEffectToOccupant();
	void RemoveCoverEffectFromOccupant();
};
