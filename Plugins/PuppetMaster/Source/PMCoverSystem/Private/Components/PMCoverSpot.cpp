// Copyright Alex Jobe


#include "Components/PMCoverSpot.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Subsystems/PMCoverSubsystem.h"


UPMCoverSpot::UPMCoverSpot()
	: CoverEffectLevel(1.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	SphereRadius = 50.f;
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

bool UPMCoverSpot::ClaimCoverSpot(AActor* InActor)
{
	if (CurrentOccupant) return false;
	if (!GetOwner()->HasAuthority()) return false;
	
	CurrentOccupant = InActor;
	ApplyCoverEffectToOccupant();

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->AddActiveCoverSpot(this);
	}
	
	return true;
}

void UPMCoverSpot::ReleaseCoverSpot()
{
	if (!GetOwner()->HasAuthority()) return;
	
	RemoveCoverEffectFromOccupant();
	CurrentOccupant = nullptr;

	if (UPMCoverSubsystem* CoverSubsystem = UPMCoverSubsystem::GetSubsystem(this); ensure(CoverSubsystem))
	{
		CoverSubsystem->RemoveActiveCoverSpot(this);
	}
}

void UPMCoverSpot::ApplyCoverEffectToOccupant()
{
	if (!ensure(CurrentOccupant) || !CoverEffectClass || ActiveCoverEffectHandle.IsSet()) return;
	if (!GetOwner()->HasAuthority()) return;

	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentOccupant))
	{
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CoverEffectClass, CoverEffectLevel, ContextHandle);
		ActiveCoverEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
	}
}

void UPMCoverSpot::RemoveCoverEffectFromOccupant()
{
	if (!ensure(CurrentOccupant) || !ActiveCoverEffectHandle.IsSet()) return;
	if (!GetOwner()->HasAuthority()) return;

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentOccupant);
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveCoverEffectHandle.GetValue());
	}

	ActiveCoverEffectHandle.Reset();
}
