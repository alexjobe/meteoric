// Copyright Alex Jobe


#include "Components/PMCoverUserComponent.h"

#include "Components/PMCoverSpot.h"
#include "Subsystems/PMCoverSubsystem.h"


UPMCoverUserComponent::UPMCoverUserComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	// Tick is only used for drawing debug helpers - should only be enabled in development builds
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
#endif
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

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
void UPMCoverUserComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	/*
	 * Tick is only used for drawing debug helpers - should only be enabled in development builds
	 */
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this);
	if (!ensure(CoverSubsystem)) return;

	// Draw all active cover spots
	for (const auto& CoverSpots = CoverSubsystem->GetActiveCoverSpots(); const UPMCoverSpot* Spot : CoverSpots)
	{
		if (const AActor* SpotOccupant = Spot ? Spot->GetCurrentOccupant() : nullptr; ensure(SpotOccupant))
		{
			DrawDebugSphere(GetWorld(), Spot->GetComponentLocation(), Spot->GetScaledSphereRadius(),
				16, FColor::Yellow, false, -1, 0, 2.f);

			DrawDebugLine(GetWorld(), Spot->GetComponentLocation(), SpotOccupant->GetActorLocation(),
				FColor::Green, false, -1, 0, 2.f);
		}
	}
}

void UPMCoverUserComponent::DrawCoverDebug(const bool bShouldDraw)
{
	SetComponentTickEnabled(bShouldDraw);
}
#endif

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
