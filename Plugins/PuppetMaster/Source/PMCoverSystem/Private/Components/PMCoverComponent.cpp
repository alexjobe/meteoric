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
		Spot->InitializeCoverSpot(CoverEffectClass, CoverEffectLevel);
		Spot->SetValidCoverHalfAngle(ValidCoverHalfAngle);
		Spot->OnOccupantChanged().AddUObject(this, &UPMCoverComponent::CoverSpot_OnOccupantChanged);
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
	const int32 ClaimedSpots = Occupants.Num() + Reservations.Num();
	const int32 OpenSpots = CoverSpots.Num() - ClaimedSpots;
	if (!ensure(OpenSpots >= 0)) return false;
	return Occupants.Contains(InQuerier) || (OpenSpots > 0 && ClaimedSpots < MaxOccupants);
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

void UPMCoverComponent::CoverSpot_OnOccupantChanged(const AActor* NewOccupant, const AActor* OldOccupant)
{
	UpdateActorPropertySet(Occupants, NewOccupant, OldOccupant);
}

void UPMCoverComponent::CoverSpot_OnReservationChanged(const AActor* NewReserver, const AActor* OldReserver)
{
	UpdateActorPropertySet(Reservations, NewReserver, OldReserver);
}

void UPMCoverComponent::UpdateActorPropertySet(TSet<AActor const*>& InSet, const AActor* InNewActor, const AActor* InOldActor)
{
	if (InOldActor != nullptr && InSet.Contains(InOldActor))
	{
		InSet.Remove(InOldActor);
	}
	if (InNewActor != nullptr)
	{
		InSet.Emplace(InNewActor);
	}
}
