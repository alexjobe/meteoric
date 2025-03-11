// Copyright Alex Jobe


#include "Components/PMCoverSpot.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Subsystems/PMCoverSubsystem.h"


UPMCoverSpot::UPMCoverSpot()
	: CoverEffectLevel(1.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPMCoverSpot::InitializeCoverSpot(const TSubclassOf<UGameplayEffect>& InCoverEffectClass, const float InCoverEffectLevel)
{
	CoverEffectClass = InCoverEffectClass;
	CoverEffectLevel = InCoverEffectLevel;
}

float UPMCoverSpot::GetCoverScore(const FVector& InTargetLocation) const
{
	FVector DirectionToTarget = InTargetLocation - GetComponentLocation();
	DirectionToTarget.Normalize();
	return FVector::DotProduct(GetForwardVector(), DirectionToTarget);
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

	Reserver = InActor;
	ReservationChangedEvent.Broadcast(Reserver);

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddReservedCoverSpot(this);
	}

	// Reserve this spot for the specified time
	TWeakObjectPtr<UPMCoverSpot> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(ReservationTimerHandle, [WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis.Get()->CancelReservation();
		}
	}, InReservationDuration, false);

	return true;
}

void UPMCoverSpot::CancelReservation()
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsReserved()) return;
	
	Reserver = nullptr;
	ReservationChangedEvent.Broadcast(nullptr);

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveReservedCoverSpot(this);
	}
}

bool UPMCoverSpot::Occupy(AActor* InActor)
{
	if (IsOccupied()) return false;
	if (!GetOwner()->HasAuthority()) return false;
	
	Occupant = InActor;
	ApplyCoverEffectToOccupant();

	// If an actor is occupying this spot, other actors cannot reserve it, and any existing reservations are invalid
	// This prevents bots from reserving a spot if a player occupies it
	CancelReservation();

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddOccupiedCoverSpot(this);
	}
	
	return true;
}

void UPMCoverSpot::Unoccupy()
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsOccupied()) return;
	
	RemoveCoverEffectFromOccupant();
	Occupant = nullptr;

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveOccupiedCoverSpot(this);
	}
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
