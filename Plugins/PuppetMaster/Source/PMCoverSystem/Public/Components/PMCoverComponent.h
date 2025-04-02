// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PMCoverComponent.generated.h"

/*
 * Added to actors that can be used as cover. A single piece of cover can have multiple cover spots (PMCoverSpot) that
 * are claimable by actors with a UPMCoverUserComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PMCOVERSYSTEM_API UPMCoverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/* Effect applied to actor using this cover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TSubclassOf<class UGameplayEffect> CoverEffectClass;

	/* CoverEffectClass level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float CoverEffectLevel;

	/* Maximum number of actors that can occupy this cover, regardless of how many cover spots are available */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover")
	int32 MaxOccupants;

	/*
	 * Half angle of the cone with vertex at the cover spot, and axis along the cover spot's forward vector. Cover is
	 * valid against target if target is inside the cone.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cover", meta = (ClampMin = 0, ClampMax = 90))
	float ValidCoverHalfAngle;
	
	UPMCoverComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

	/*
	 * Returns the best cover spot from the target location
	 * @param InTargetLocation: Location to test against cover
	 * @param InQuerier: Pawn looking for cover
	 * @param bTestCoverSpotNavigable: Test if querier can reach cover spot -- potentially expensive
	 */
	class UPMCoverSpot* GetBestCoverSpot(const FVector& InTargetLocation, AActor* InQuerier, const bool bTestCoverSpotNavigable = true);

	/* Returns true if cover spots are available and number of current occupants is less than MaxOccupants */
	bool IsCoverAvailable(const AActor* InQuerier) const;

	/* Is the querier an occupant or reserver? */
	bool IsClaimant(const AActor* InQuerier) const;

private:
	/* Cover spots associated with this cover */
	UPROPERTY(Transient)
	TArray<UPMCoverSpot*> CoverSpots;

	UPROPERTY(Transient)
	TSet<const UPMCoverSpot*> OccupiedSpots;

	UPROPERTY(Transient)
	TSet<const UPMCoverSpot*> ReservedSpots;

	/* Cache current occupants for quick lookup */
	UPROPERTY(Transient)
	TSet<const AActor*> Occupants;

	/* Cache current reservations for quick lookup */
	UPROPERTY(Transient)
	TSet<const AActor*> Reservers;

	/* Test if querier can reach cover spot -- potentially expensive */
	UPMCoverSpot* ChooseBestNavigableSpot(UPMCoverSpot* InCandidate, TArray<TTuple<UPMCoverSpot*, float>>& InScoredSpots, AActor* InQuerier);

	void CoverSpot_OnOccupantChanged(const UPMCoverSpot* InCoverSpot, const AActor* InNewOccupant, const AActor* InOldOccupant);
	void CoverSpot_OnReservationChanged(const UPMCoverSpot* InCoverSpot, const AActor* InNewReserver, const AActor* InOldReserver);
	static void UpdateCoverSpotSet(TSet<const UPMCoverSpot*>& CoverSpotSet, const UPMCoverSpot* CoverSpot, const AActor* NewClaimant);
	static void UpdateClaimantSet(TSet<const AActor*>& ClaimantSet, TSet<const UPMCoverSpot*>& CoverSpotSet, const AActor* NewClaimant, const AActor* OldClaimant);
};
