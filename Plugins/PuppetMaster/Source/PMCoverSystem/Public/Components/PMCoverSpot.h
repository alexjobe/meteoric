// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Components/SphereComponent.h"
#include "PMCoverSpot.generated.h"

DECLARE_EVENT_TwoParams(UPMCoverSpot, FOnActorPropertyChangedEvent, const AActor* NewActor, const AActor* OldActor)

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
	void SetValidCoverHalfAngle(const float InHalfAngle);

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
	FOnActorPropertyChangedEvent& OnReservationChangedEvent() { return ReservationChangedEvent; }
	FOnActorPropertyChangedEvent& OnOccupantChanged() { return OccupantChangedEvent; }

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
	FOnActorPropertyChangedEvent ReservationChangedEvent;
	FOnActorPropertyChangedEvent OccupantChangedEvent;

	/*
	 * Half angle of the cone with vertex at the cover spot, and axis along the cover spot's forward vector. Cover is
	 * valid against target if target is inside the cone.
	 */
	float ValidCoverHalfAngle;

	// Cosine of the ValidCoverHalfAngle
	float MinCoverScore;

	void ApplyCoverEffectToOccupant();
	void RemoveCoverEffectFromOccupant();
};
