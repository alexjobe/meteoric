// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/SphereComponent.h"
#include "PMCoverSpot.generated.h"

DECLARE_EVENT_OneParam(UPMCoverSpot, FOnClaimChangedEvent, const AActor* Claimant)

/*
 * Represents a single cover spot that can be occupied by actors with a PMCoverUserComponent. Intended to be attached to
 * actors with a PMCoverComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PMCOVERSYSTEM_API UPMCoverSpot : public USphereComponent
{
	GENERATED_BODY()

public:
	UPMCoverSpot();
	
	void InitializeCoverSpot(const TSubclassOf<class UGameplayEffect>& InCoverEffectClass, float InCoverEffectLevel);

	/*
	 * Cover score is the dot product between the cover spot's forward vector and the direction vector from the cover
	 * spot to the target location
	 */
	float GetCoverScore(const FVector& InTargetLocation) const;
	bool IsClaimed() const { return Claimant != nullptr; }
	bool IsOccupied() const { return Occupant != nullptr; }
	bool ClaimCoverSpot(AActor* InActor, const float InClaimDuration = 10.f);
	void UnclaimCoverSpot();
	bool OccupyCoverSpot(AActor* InActor);
	void UnoccupyCoverSpot();
	AActor* GetClaimant() const { return Claimant; }
	AActor* GetOccupant() const { return Occupant; }
	FOnClaimChangedEvent& OnClaimChangedEvent() { return ClaimChangedEvent; }

private:
	/* Actor currently occupying this cover spot */
	UPROPERTY(Transient)
	TObjectPtr<AActor> Occupant;

	/* 
	 * Actor that has claimed this spot - used to prevent multiple AI actors from attempting to move to the same spot
	 * Claiming a spot is declaring intention to use it, but does not prevent other actors from occupying it
	 */
	UPROPERTY(Transient)
	TObjectPtr<AActor> Claimant;
	
	/* Effect applied to actor using this cover */
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> CoverEffectClass;

	/* CoverEffectClass level */
	float CoverEffectLevel;

	TOptional<FActiveGameplayEffectHandle> ActiveCoverEffectHandle;

	FTimerHandle ClaimTimerHandle;
	FOnClaimChangedEvent ClaimChangedEvent;

	void ApplyCoverEffectToOccupant();
	void RemoveCoverEffectFromOccupant();
};
