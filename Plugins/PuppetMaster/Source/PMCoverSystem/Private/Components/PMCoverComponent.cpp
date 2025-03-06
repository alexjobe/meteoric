// Copyright Alex Jobe


#include "Components/PMCoverComponent.h"

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

UPMCoverSpot* UPMCoverComponent::GetBestCoverSpot(const FVector& InTargetLocation)
{
	UPMCoverSpot* BestCoverSpot = nullptr;
	float BestCoverScore = 0.f;
	for (const auto Spot : CoverSpots)
	{
		float Score = Spot->GetCoverScore(InTargetLocation);
		if (Score > BestCoverScore)
		{
			BestCoverScore = Score;
			BestCoverSpot = Spot;
		}
	}
	return BestCoverSpot;
}
