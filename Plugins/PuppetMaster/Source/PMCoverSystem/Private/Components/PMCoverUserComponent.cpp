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

bool UPMCoverUserComponent::ClaimCoverSpot(UPMCoverSpot* CoverSpot)
{
	if (!ensure(CoverSpot)) return false;
	if (ClaimedCoverSpot == CoverSpot) return false;
	
	// If we try to claim a spot we already occupy, just return early
	if (CoverSpot->GetOccupant() == GetOwner()) return true;

	UnclaimCoverSpot();
	
	if (CoverSpot->ClaimCoverSpot(GetOwner()))
	{
		ClaimedCoverSpot = CoverSpot;
		ClaimedCoverSpot->OnClaimChangedEvent().AddUObject(this, &UPMCoverUserComponent::ClaimedCoverSpot_OnClaimChangedEvent);
		return true;
	}
	return false;
}

void UPMCoverUserComponent::ReleaseCoverSpots()
{
	if (OccupiedCoverSpot)
	{
		OccupiedCoverSpot->UnoccupyCoverSpot();
		OccupiedCoverSpot = nullptr;
	}
	UnclaimCoverSpot();
}

void UPMCoverUserComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ReleaseCoverSpots();
	Super::EndPlay(EndPlayReason);
}

void UPMCoverUserComponent::UnclaimCoverSpot()
{
	if (ClaimedCoverSpot)
	{
		ClaimedCoverSpot->OnClaimChangedEvent().RemoveAll(this);
		ClaimedCoverSpot->UnclaimCoverSpot();
		ClaimedCoverSpot = nullptr;
	}
}

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
void UPMCoverUserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	/*
	 * Tick is only used for drawing debug helpers - should only be enabled in development builds
	 */
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this);
	if (!ensure(CoverSubsystem)) return;

	// Draw all occupied cover spots
	for (const auto& CoverSpots = CoverSubsystem->GetOccupiedCoverSpots(); const UPMCoverSpot* Spot : CoverSpots)
	{
		if (const AActor* Occupant = Spot ? Spot->GetOccupant() : nullptr; ensure(Occupant))
		{
			DrawDebugSphere(GetWorld(), Spot->GetComponentLocation(), Spot->GetScaledSphereRadius(),
				16, FColor::Green, false, -1, 0, 2.f);

			DrawDebugLine(GetWorld(), Spot->GetComponentLocation(), Occupant->GetActorLocation(),
				FColor::Green, false, -1, 0, 2.f);
		}
	}

	// Draw all claimed cover spots
	for (const auto& CoverSpots = CoverSubsystem->GetClaimedCoverSpots(); const UPMCoverSpot* Spot : CoverSpots)
	{
		if (const AActor* Claimant = Spot ? Spot->GetClaimant() : nullptr; ensure(Claimant))
		{
			DrawDebugSphere(GetWorld(), Spot->GetComponentLocation(), Spot->GetScaledSphereRadius(),
				16, FColor::Yellow, false, -1, 0, 2.f);

			DrawDebugLine(GetWorld(), Spot->GetComponentLocation(), Claimant->GetActorLocation(),
				FColor::Yellow, false, -1, 0, 2.f);
		}
	}
}

void UPMCoverUserComponent::DrawCoverDebug(const bool bShouldDraw)
{
	SetComponentTickEnabled(bShouldDraw);
}
#endif

void UPMCoverUserComponent::ClaimedCoverSpot_OnClaimChangedEvent(const AActor* Claimant)
{
	if (ensure(ClaimedCoverSpot) && GetOwner() != Claimant)
	{
		ClaimedCoverSpot->OnClaimChangedEvent().RemoveAll(this);
		ClaimedCoverSpot = nullptr;
	}
}

void UPMCoverUserComponent::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UPMCoverSpot* CoverSpot = Cast<UPMCoverSpot>(OtherComp);
	if (!CoverSpot) return;

	if (CoverSpot->OccupyCoverSpot(GetOwner()))
	{
		if (OccupiedCoverSpot && OccupiedCoverSpot != CoverSpot)
		{
			OccupiedCoverSpot->UnoccupyCoverSpot();
		}
		OccupiedCoverSpot = CoverSpot;
	}
}

void UPMCoverUserComponent::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OccupiedCoverSpot && OccupiedCoverSpot == OtherComp)
	{
		OccupiedCoverSpot->UnoccupyCoverSpot();
		OccupiedCoverSpot = nullptr;
	}
}
