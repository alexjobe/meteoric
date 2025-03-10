// Copyright Alex Jobe


#include "Components/PMCoverComponent.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/PMCoverSpot.h"


UPMCoverComponent::UPMCoverComponent()
	: CoverEffectLevel(1.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMCoverComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetComponents(UPMCoverSpot::StaticClass(), CoverSpots);
	for (const auto Spot : CoverSpots)
	{
		Spot->InitializeCoverSpot(CoverEffectClass, CoverEffectLevel);
	}
}

UPMCoverSpot* UPMCoverComponent::GetBestCoverSpot(const FVector& InTargetLocation, const FVector& InQuerierLocation, const bool bTestCoverSpotNavigable)
{
	UPMCoverSpot* BestCoverSpot = nullptr;
	float BestCoverScore = 0.f;

	TArray<TTuple<UPMCoverSpot*, float>> ScoredSpots;
	
	for (const auto Spot : CoverSpots)
	{
		if (Spot->IsOccupied() || Spot->IsClaimed()) continue;
		float Score = Spot->GetCoverScore(InTargetLocation);
		if (Score > BestCoverScore)
		{
			BestCoverScore = Score;
			BestCoverSpot = Spot;
		}
		ScoredSpots.Emplace(TTuple<UPMCoverSpot*, float>(Spot, Score));
	}

	if (bTestCoverSpotNavigable)
	{
		BestCoverSpot = ChooseBestNavigableSpot(BestCoverSpot, ScoredSpots, InQuerierLocation);
	}
	
	return BestCoverSpot;
}

UPMCoverSpot* UPMCoverComponent::ChooseBestNavigableSpot(UPMCoverSpot* InCandidate, TArray<TTuple<UPMCoverSpot*, float>>& InScoredSpots, const FVector& InQuerierLocation)
{
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!ensure(NavSystem) || !ensure(InCandidate)) return nullptr;

	// Test candidate -- if valid, we don't have to sort scored spots
	const UNavigationPath* CandidatePath = NavSystem->FindPathToLocationSynchronously(this, InQuerierLocation, InCandidate->GetComponentLocation());
	if (CandidatePath && CandidatePath->IsValid())
	{
		return InCandidate;
	}

	// If candidate isn't valid, sort spots from highest to lowest score
	InScoredSpots.Sort([](const TTuple<UPMCoverSpot*, float>& A, const TTuple<UPMCoverSpot*, float>& B)
	{
		return A.Value > B.Value;
	});

	// Find first reachable spot
	for (const auto& Pair : InScoredSpots)
	{
		if (!ensure(Pair.Key)) continue;
		const UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(this, InQuerierLocation, Pair.Key->GetComponentLocation());
		if (Path && Path->IsValid())
		{
			return Pair.Key;
		}
	}
	
	return nullptr;
}
