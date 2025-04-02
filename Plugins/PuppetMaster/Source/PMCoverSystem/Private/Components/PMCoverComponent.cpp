// Copyright Alex Jobe


#include "Components/PMCoverComponent.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/PMCoverSpot.h"


UPMCoverComponent::UPMCoverComponent()
	: CoverEffectLevel(1.f)
	, MaxOccupants(1)
	, ValidCoverHalfAngle(45.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMCoverComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetComponents(UPMCoverSpot::StaticClass(), CoverSpots);
	for (const auto Spot : CoverSpots)
	{
		FIsCoverAvailableDelegate IsCoverAvailableDelegate;
		IsCoverAvailableDelegate.BindUObject(this, &UPMCoverComponent::IsCoverAvailable);
		Spot->InitializeCoverSpot(CoverEffectClass, CoverEffectLevel, IsCoverAvailableDelegate);
		Spot->SetValidCoverHalfAngle(ValidCoverHalfAngle);
		Spot->OnOccupantChangedEvent().AddUObject(this, &UPMCoverComponent::CoverSpot_OnOccupantChanged);
		Spot->OnReservationChangedEvent().AddUObject(this, &UPMCoverComponent::CoverSpot_OnReservationChanged);
	}
}

UPMCoverSpot* UPMCoverComponent::GetBestCoverSpot(const FVector& InTargetLocation, AActor* InQuerier, const bool bTestCoverSpotNavigable)
{
	UPMCoverSpot* BestCoverSpot = nullptr;
	float BestCoverScore = 0.f;

	TArray<TTuple<UPMCoverSpot*, float>> ScoredSpots;
	
	for (const auto Spot : CoverSpots)
	{
		if (!Spot->CanBeReserved(InQuerier)) continue;
		float Score = Spot->GetCoverScore(InTargetLocation);
		if (Score > BestCoverScore)
		{
			BestCoverScore = Score;
			BestCoverSpot = Spot;
		}
		ScoredSpots.Emplace(TTuple<UPMCoverSpot*, float>(Spot, Score));
	}

	if (BestCoverSpot && bTestCoverSpotNavigable)
	{
		BestCoverSpot = ChooseBestNavigableSpot(BestCoverSpot, ScoredSpots, InQuerier);
	}
	
	return BestCoverSpot;
}

bool UPMCoverComponent::IsCoverAvailable(const AActor* InQuerier) const
{
	if (!ensure(InQuerier)) return false;
	const int32 ClaimedSpots = OccupiedSpots.Num() + ReservedSpots.Num();
	const int32 OpenSpots = CoverSpots.Num() - ClaimedSpots;
	if (!ensure(OpenSpots >= 0)) return false;
	
	return IsClaimant(InQuerier) || (OpenSpots > 0 && ClaimedSpots < MaxOccupants);
}

bool UPMCoverComponent::IsClaimant(const AActor* InQuerier) const
{
	return Occupants.Contains(InQuerier) || Reservers.Contains(InQuerier);
}

UPMCoverSpot* UPMCoverComponent::ChooseBestNavigableSpot(UPMCoverSpot* InCandidate, TArray<TTuple<UPMCoverSpot*, float>>& InScoredSpots, AActor* InQuerier)
{
	const UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!ensure(NavSystem) || !ensure(InCandidate) || !ensure(InQuerier)) return nullptr;

	// Test candidate -- if valid, we don't have to sort scored spots
	const UNavigationPath* CandidatePath = NavSystem->FindPathToLocationSynchronously(this, InQuerier->GetActorLocation(), InCandidate->GetComponentLocation(), InQuerier);
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
		const UNavigationPath* Path = NavSystem->FindPathToLocationSynchronously(this, InQuerier->GetActorLocation(), Pair.Key->GetComponentLocation(), InQuerier);
		if (Path && Path->IsValid())
		{
			return Pair.Key;
		}
	}
	
	return nullptr;
}

void UPMCoverComponent::CoverSpot_OnOccupantChanged(const UPMCoverSpot* InCoverSpot, const AActor* InNewOccupant, const AActor* InOldOccupant)
{
	if (!ensure(InCoverSpot)) return;
	UpdateCoverSpotSet(OccupiedSpots, InCoverSpot, InNewOccupant);
	UpdateClaimantSet(Occupants, OccupiedSpots, InNewOccupant, InOldOccupant);
	
}

void UPMCoverComponent::CoverSpot_OnReservationChanged(const UPMCoverSpot* InCoverSpot, const AActor* InNewReserver, const AActor* InOldReserver)
{
	if (!ensure(InCoverSpot)) return;
	UpdateCoverSpotSet(ReservedSpots, InCoverSpot, InNewReserver);
	UpdateClaimantSet(Reservers, ReservedSpots, InNewReserver, InOldReserver);
}

void UPMCoverComponent::UpdateCoverSpotSet(TSet<const UPMCoverSpot*>& CoverSpotSet, const UPMCoverSpot* CoverSpot, const AActor* NewClaimant)
{
	if (!ensure(CoverSpot)) return;
	
	if (NewClaimant == nullptr && CoverSpotSet.Contains(CoverSpot))
	{
		CoverSpotSet.Remove(CoverSpot);
	}
	
	if (NewClaimant != nullptr)
	{
		CoverSpotSet.Emplace(CoverSpot);
	}
}

void UPMCoverComponent::UpdateClaimantSet(TSet<const AActor*>& ClaimantSet, TSet<const UPMCoverSpot*>& CoverSpotSet, const AActor* NewClaimant, const AActor* OldClaimant)
{
	// Check if old claimant is still a claimant. If not, remove from set.
	if (NewClaimant == nullptr && OldClaimant != nullptr)
	{
		bool bClaimant = false;
		for (const UPMCoverSpot* Spot : CoverSpotSet)
		{
			if (ensure(Spot); Spot->GetOccupant() == OldClaimant)
			{
				bClaimant = true;
				break;
			}
		}
		if (!bClaimant && ClaimantSet.Contains(OldClaimant))
		{
			ClaimantSet.Remove(OldClaimant);
		}
	}

	// Add new claimant
	if (NewClaimant != nullptr)
	{
		ClaimantSet.Emplace(NewClaimant);
	}
}
