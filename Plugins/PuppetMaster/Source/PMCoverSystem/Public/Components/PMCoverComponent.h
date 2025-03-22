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

	/*
	 * Maximum number of actors that can occupy this cover, regardless of how many cover spots are available
	 * Used for querying if cover is available - does not prevent more actors from occupying this cover
	 * For example, if MaxOccupants is 1 and 1 of 4 spots is occupied, this cover is considered full
	 */
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
	class UPMCoverSpot* GetBestCoverSpot(const FVector& InTargetLocation, APawn* InQuerier, const bool bTestCoverSpotNavigable = true);

	/*
	 * Returns true if cover spots are available and number of current occupants is less than MaxOccupants, or if
	 * querier is already an occupant
	 */
	bool IsCoverAvailable(const AActor* InQuerier) const;

private:
	/* Cover spots associated with this cover */
	UPROPERTY(Transient)
	TArray<UPMCoverSpot*> CoverSpots;

	UPROPERTY(Transient)
	TSet<const AActor*> Occupants;

	UPROPERTY(Transient)
	TSet<const AActor*> Reservations;

	/* Test if querier can reach cover spot -- potentially expensive */
	UPMCoverSpot* ChooseBestNavigableSpot(UPMCoverSpot* InCandidate, TArray<TTuple<UPMCoverSpot*, float>>& InScoredSpots, APawn* InQuerier);

	void CoverSpot_OnOccupantChanged(const AActor* NewOccupant, const AActor* OldOccupant);
	void CoverSpot_OnReservationChanged(const AActor* NewReserver, const AActor* OldReserver);
	static void UpdateActorPropertySet(TSet<const AActor*>& InSet, const AActor* InNewActor, const AActor* InOldActor);
};
