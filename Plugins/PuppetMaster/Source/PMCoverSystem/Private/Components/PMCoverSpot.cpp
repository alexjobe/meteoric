// Copyright Alex Jobe


#include "Components/PMCoverSpot.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Subsystems/PMCoverSubsystem.h"


UPMCoverSpot::UPMCoverSpot()
	: CoverEffectLevel(1.f)
	, ValidCoverHalfAngle(45.f)
	, MinCoverScore(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetValidCoverHalfAngle(ValidCoverHalfAngle);
}

void UPMCoverSpot::InitializeCoverSpot(const TSubclassOf<UGameplayEffect>& InCoverEffectClass, const float InCoverEffectLevel)
{
	CoverEffectClass = InCoverEffectClass;
	CoverEffectLevel = InCoverEffectLevel;
}

void UPMCoverSpot::SetValidCoverHalfAngle(const float InHalfAngle)
{
	ValidCoverHalfAngle = InHalfAngle;
	MinCoverScore = FMath::Cos(FMath::DegreesToRadians(ValidCoverHalfAngle));
}

float UPMCoverSpot::GetCoverScore(const FVector& InTargetLocation) const
{
	FVector DirectionToTarget = InTargetLocation - GetComponentLocation();
	DirectionToTarget.Normalize();
	const float Score = FVector::DotProduct(GetForwardVector(), DirectionToTarget);
	return Score > MinCoverScore ? Score : 0.f;
}

bool UPMCoverSpot::CanBeReserved(const AActor* InActor) const
{
	if (!ensure(InActor)) return false;
	if (InActor == Occupant) return true;
	return !IsReserved() && !IsOccupied();
}

bool UPMCoverSpot::Reserve(AActor* InActor, const float InReservationDuration)
{
	if (!CanBeReserved(InActor)) return false;
	if (!GetOwner()->HasAuthority()) return false;
	if (InActor == Occupant) return true;

	const AActor* OldReserver = Reserver;
	Reserver = InActor;

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddReservedCoverSpot(this);
	}

	SetReservationTimer(InReservationDuration);

	ReservationChangedEvent.Broadcast(this, Reserver, OldReserver);

	return true;
}

void UPMCoverSpot::CancelReservation()
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsReserved()) return;

	const AActor* OldReserver = Reserver;
	Reserver = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(ReservationTimerHandle);

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveReservedCoverSpot(this);
	}
	
	ReservationChangedEvent.Broadcast(this, Reserver, OldReserver);
}

bool UPMCoverSpot::RenewReservation(AActor* InActor, const float InReservationDuration)
{
	if (!ensure(InActor) || InActor != Reserver) return false;
	if (!GetOwner()->HasAuthority()) return false;
	if (InActor == Occupant) return true;

	GetWorld()->GetTimerManager().ClearTimer(ReservationTimerHandle);
	SetReservationTimer(InReservationDuration);
	return true;
}

bool UPMCoverSpot::Occupy(AActor* InActor)
{
	if (IsOccupied()) return false;
	if (!GetOwner()->HasAuthority()) return false;

	const AActor* OldOccupant = Occupant;
	Occupant = InActor;
	ApplyCoverEffectToOccupant();

	// If an actor is occupying this spot, other actors cannot reserve it, and any existing reservations are invalid
	// This prevents bots from reserving a spot if a player occupies it
	CancelReservation();

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddOccupiedCoverSpot(this);
	}

	OccupantChangedEvent.Broadcast(this, Occupant, OldOccupant);
	
	return true;
}

void UPMCoverSpot::Unoccupy()
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsOccupied()) return;
	
	RemoveCoverEffectFromOccupant();

	const AActor* OldOccupant = Occupant;
	Occupant = nullptr;

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveOccupiedCoverSpot(this);
	}

	OccupantChangedEvent.Broadcast(this, Occupant, OldOccupant);
}

void UPMCoverSpot::ApplyCoverEffectToOccupant()
{
	if (!ensure(Occupant) || !CoverEffectClass || ActiveCoverEffectHandle.IsSet()) return;
	if (!GetOwner()->HasAuthority()) return;

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Occupant))
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CoverEffectClass, CoverEffectLevel, ContextHandle);
		ActiveCoverEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
	}
}

void UPMCoverSpot::RemoveCoverEffectFromOccupant()
{
	if (!ensure(Occupant) || !ActiveCoverEffectHandle.IsSet()) return;
	if (!GetOwner()->HasAuthority()) return;

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Occupant); ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveCoverEffectHandle.GetValue());
	}

	ActiveCoverEffectHandle.Reset();
}

void UPMCoverSpot::SetReservationTimer(const float InReservationDuration)
{
	// Reserve this spot for the specified time
	TWeakObjectPtr<UPMCoverSpot> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(ReservationTimerHandle, [WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis.Get()->CancelReservation();
		}
	}, InReservationDuration, false);
}
