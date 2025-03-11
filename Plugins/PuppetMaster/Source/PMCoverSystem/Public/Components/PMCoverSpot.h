// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/SphereComponent.h"
#include "PMCoverSpot.generated.h"

DECLARE_EVENT_OneParam(UPMCoverSpot, FOnReservationChangedEvent, const AActor* Reserver)

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
	bool CanBeReserved(const AActor* InActor) const;
	bool IsReserved() const { return Reserver != nullptr; }
	bool IsOccupied() const { return Occupant != nullptr; }
	bool Reserve(AActor* InActor, const float InReservationDuration = 10.f);
	void CancelReservation();
	bool Occupy(AActor* InActor);
	void Unoccupy();
	AActor* GetReservation() const { return Reserver; }
	AActor* GetOccupant() const { return Occupant; }
	FOnReservationChangedEvent& OnReservationChangedEvent() { return ReservationChangedEvent; }

private:
	/* Actor currently occupying this cover spot */
	UPROPERTY(Transient)
	TObjectPtr<AActor> Occupant;

	/* 
	 * Actor that has reserved this spot - used to prevent multiple AI actors from attempting to move to the same spot
	 * Reserving a spot is declaring intention to use it, but does not prevent other actors from occupying it
	 */
	UPROPERTY(Transient)
	TObjectPtr<AActor> Reserver;
	
	/* Effect applied to actor using this cover */
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> CoverEffectClass;

	/* CoverEffectClass level */
	float CoverEffectLevel;

	TOptional<FActiveGameplayEffectHandle> ActiveCoverEffectHandle;

	FTimerHandle ReservationTimerHandle;
	FOnReservationChangedEvent ReservationChangedEvent;

	void ApplyCoverEffectToOccupant();
	void RemoveCoverEffectFromOccupant();
};
