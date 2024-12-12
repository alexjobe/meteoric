// Copyright Alex Jobe


#include "METWeaponAmmoComponent.h"

#include "METAmmoManager.h"
#include "GameFramework/Character.h"

UMETWeaponAmmoComponent::UMETWeaponAmmoComponent()
	: MaxCount(20)
	, AmmoCount(20)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETWeaponAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	AmmoCount = MaxCount;
}

int32 UMETWeaponAmmoComponent::Reload()
{
	UMETAmmoManager* AmmoManager = OwningCharacter ? OwningCharacter->FindComponentByClass<UMETAmmoManager>() : nullptr;
	if (!ensure(AmmoManager)) return AmmoCount;

	const int32 ReloadAmount = AmmoManager->TryConsumeAmmo(CurrentAmmoType, FMath::Max(0, MaxCount - AmmoCount));
	AmmoCount += ReloadAmount;
	return AmmoCount;
}

bool UMETWeaponAmmoComponent::TryConsumeAmmo(const int32 InConsumeCount)
{
	if (InConsumeCount < 0) return false;
	bool bConsumed = false;
	if (const int32 NewCount = AmmoCount - InConsumeCount; NewCount >= 0)
	{
		AmmoCount = NewCount;
		bConsumed = true;
	}
	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("Ammo Remaining: %i"), AmmoCount));
	return bConsumed;
}

void UMETWeaponAmmoComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
}

void UMETWeaponAmmoComponent::OnWeaponUnequipped()
{
	OwningCharacter = nullptr;
}
