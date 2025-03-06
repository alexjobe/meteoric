// Copyright Alex Jobe


#include "Components/PMCoverUserComponent.h"

#include "Components/PMCoverSpot.h"


UPMCoverUserComponent::UPMCoverUserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMCoverUserComponent::InitializeCoverUser(UPrimitiveComponent* OverlappedComponent)
{
	if (ensure(OverlappedComponent))
	{
		OverlappedComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &UPMCoverUserComponent::OnComponentBeginOverlap);
		OverlappedComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &UPMCoverUserComponent::OnComponentEndOverlap);
	}
}

void UPMCoverUserComponent::ReleaseCoverSpot() const
{
	if (ClaimedCoverSpot)
	{
		ClaimedCoverSpot->ReleaseCoverSpot();
	}
}

void UPMCoverUserComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UPMCoverSpot* CoverSpot = Cast<UPMCoverSpot>(OtherComp);
	if (!CoverSpot) return;

	if (CoverSpot->ClaimCoverSpot(GetOwner()))
	{
		if (ClaimedCoverSpot && ClaimedCoverSpot != CoverSpot)
		{
			ClaimedCoverSpot->ReleaseCoverSpot();
		}
		ClaimedCoverSpot = CoverSpot;
	}
}

void UPMCoverUserComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ClaimedCoverSpot && ClaimedCoverSpot == OtherComp)
	{
		ClaimedCoverSpot->ReleaseCoverSpot();
		ClaimedCoverSpot = nullptr;
	}
}
