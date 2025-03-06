// Copyright Alex Jobe


#include "Components/PMCoverSpot.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"


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

bool UPMCoverSpot::ClaimCoverSpot(AActor* InActor)
{
	if (CurrentOccupant) return false;
	CurrentOccupant = InActor;
	ApplyCoverEffectToOccupant();
	DrawDebugSphere(GetWorld(), GetComponentLocation(), SphereRadius, 16, FColor::Yellow, false, 5.f, 0, 2.f);
	return true;
}

void UPMCoverSpot::ReleaseCoverSpot()
{
	RemoveCoverEffectFromOccupant();
	CurrentOccupant = nullptr;
}

void UPMCoverSpot::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UPMCoverSpot::ApplyCoverEffectToOccupant()
{
	if (!ensure(CurrentOccupant) || !CoverEffectClass || ActiveCoverEffectHandle.IsSet()) return;

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

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CurrentOccupant);
	if (ensure(AbilitySystemComponent))
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveCoverEffectHandle.GetValue());
	}

	ActiveCoverEffectHandle.Reset();
}
