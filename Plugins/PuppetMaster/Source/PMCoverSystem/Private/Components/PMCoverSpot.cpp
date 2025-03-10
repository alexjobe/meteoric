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

bool UPMCoverSpot::ClaimCoverSpot(AActor* InActor, const float InClaimDuration)
{
	if (!ensure(InActor) || IsOccupied() || IsClaimed()) return false;
	if (!GetOwner()->HasAuthority()) return false;

	Claimant = InActor;
	ClaimChangedEvent.Broadcast(Claimant);

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddClaimedCoverSpot(this);
	}

	// Set a timer for claim duration, "reserving" this spot for the specified time
	TWeakObjectPtr<UPMCoverSpot> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(ClaimTimerHandle, [WeakThis]()
	{
		if (WeakThis.IsValid())
		{
			WeakThis.Get()->UnclaimCoverSpot();
		}
	}, InClaimDuration, false);

	return true;
}

void UPMCoverSpot::UnclaimCoverSpot()
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsClaimed()) return;
	
	Claimant = nullptr;
	ClaimChangedEvent.Broadcast(nullptr);

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveClaimedCoverSpot(this);
	}
}

bool UPMCoverSpot::OccupyCoverSpot(AActor* InActor)
{
	if (IsOccupied()) return false;
	if (!GetOwner()->HasAuthority()) return false;
	
	Occupant = InActor;
	ApplyCoverEffectToOccupant();

	// If an actor is occupying this spot, other actors cannot claim it, and any existing claims are invalid
	UnclaimCoverSpot();

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddOccupiedCoverSpot(this);
	}
	
	return true;
}

void UPMCoverSpot::UnoccupyCoverSpot()
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
