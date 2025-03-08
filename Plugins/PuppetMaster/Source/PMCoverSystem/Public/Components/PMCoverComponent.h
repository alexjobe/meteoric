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
	
	UPMCoverComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

	/*
	 * Returns the best cover spot from the target location
	 * @param InTargetLocation: Location to test against cover
	 * @param InQuerierLocation: Location of querier looking for cover
	 * @param bTestCoverSpotNavigable: Test if querier can reach cover spot -- potentially expensive
	 */
	UFUNCTION(BlueprintCallable)
	class UPMCoverSpot* GetBestCoverSpot(const FVector& InTargetLocation, const FVector& InQuerierLocation, const bool bTestCoverSpotNavigable = true);

private:
	/* Cover spots associated with this cover */
	UPROPERTY(Transient)
	TArray<UPMCoverSpot*> CoverSpots;

	/* Test if querier can reach cover spot -- potentially expensive */
	UPMCoverSpot* ChooseBestNavigableSpot(UPMCoverSpot* InCandidate, TArray<TTuple<UPMCoverSpot*, float>>& InScoredSpots, const FVector& InQuerierLocation);
};
