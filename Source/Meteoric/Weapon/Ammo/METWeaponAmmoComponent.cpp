// Copyright Alex Jobe


#include "METWeaponAmmoComponent.h"

#include "METAmmoDataAsset.h"
#include "METAmmoManager.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponAmmoComponent::UMETWeaponAmmoComponent()
	: MaxAmmo(20)
	, AmmoCount(20)
	, SelectedAmmoSlot(0)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETWeaponAmmoComponent::BeginPlay()
{
	Super::BeginPlay();
	AmmoCount = MaxAmmo;
}

int32 UMETWeaponAmmoComponent::Reload()
{
	if (!ensure(AmmoManager)) return AmmoCount;

	const int32 ReloadAmount = AmmoManager->TryConsumeReserveAmmo(CurrentAmmoType, FMath::Max(0, MaxAmmo - AmmoCount));
	AmmoCount += ReloadAmount;
	AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
	return AmmoCount;
}

void UMETWeaponAmmoComponent::CycleAmmo(const bool bInForward)
{
	if (!ensure(AmmoManager)) return;
	if (AmmoTypes.Num() <= 1) return;

	AmmoManager->AddReserveAmmo(CurrentAmmoType, AmmoCount);
	AmmoCount = 0;
	AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);

	int NewSlot = SelectedAmmoSlot + (bInForward ? 1 : -1);
	if(NewSlot < 0) NewSlot = AmmoTypes.Num() - 1;
	if(NewSlot >= AmmoTypes.Num()) NewSlot = 0;

	SelectedAmmoSlot = NewSlot;
	CurrentAmmoType = AmmoTypes[SelectedAmmoSlot];
	
	AmmoManager->WeaponAmmoTypeChanged(GetOwner(),CurrentAmmoType);
}

float UMETWeaponAmmoComponent::GetAmmoPercentage() const
{
	if (ensure(MaxAmmo > 0))
	{
		const float AmmoPercent = static_cast<float>(AmmoCount) / MaxAmmo * 100;
		return AmmoPercent;
	}
	return 0.f;
}

float UMETWeaponAmmoComponent::GetImpactDamage() const
{
	if (CurrentAmmoType)
	{
		return CurrentAmmoType->ImpactDamage;
	}
	return 0.f;
}

float UMETWeaponAmmoComponent::GetDelayedDamage() const
{
	if (CurrentAmmoType)
	{
		return CurrentAmmoType->DelayedDamage;
	}
	return 0.f;
}

TSubclassOf<UGameplayEffect> UMETWeaponAmmoComponent::GetImpactDamageEffectClass() const
{
	if (CurrentAmmoType)
	{
		return CurrentAmmoType->ImpactDamageEffectClass;
	}
	return nullptr;
}

TSubclassOf<UGameplayEffect> UMETWeaponAmmoComponent::GetDelayedDamageEffectClass() const
{
	if (CurrentAmmoType)
	{
		return CurrentAmmoType->DelayedDamageEffectClass;
	}
	return nullptr;
}

bool UMETWeaponAmmoComponent::TryConsumeAmmo(const int32 InConsumeCount)
{
	if (!ensure(AmmoManager)) return false;
	if (InConsumeCount < 0) return false;
	
	bool bConsumed = false;
	if (const int32 NewCount = AmmoCount - InConsumeCount; NewCount >= 0)
	{
		AmmoCount = NewCount;
		bConsumed = true;
		AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
	}
	return bConsumed;
}

void UMETWeaponAmmoComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	if (!ensure(InOwningCharacter)) return;

	OwningCharacter = InOwningCharacter;
	AmmoManager = OwningCharacter->FindComponentByClass<UMETAmmoManager>();
	if (ensure(AmmoManager))
	{
		AmmoManager->WeaponAmmoChanged(AmmoCount, MaxAmmo);
		AmmoManager->WeaponAmmoTypeChanged(GetOwner(),CurrentAmmoType);
	}
}

void UMETWeaponAmmoComponent::OnWeaponUnequipped()
{
	if (AmmoManager)
	{
		AmmoManager->WeaponAmmoChanged(0, 0);
		AmmoManager->WeaponAmmoTypeChanged(nullptr, nullptr);
	}
	OwningCharacter = nullptr;
	AmmoManager = nullptr;
}

void UMETWeaponAmmoComponent::OnRep_OwningCharacter(ACharacter* InOldOwner)
{
	if (OwningCharacter)
	{
		AmmoManager = OwningCharacter->FindComponentByClass<UMETAmmoManager>();
	}
	else
	{
		AmmoManager = nullptr;
	}
}

void UMETWeaponAmmoComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMETWeaponAmmoComponent, CurrentAmmoType);
	DOREPLIFETIME(UMETWeaponAmmoComponent, AmmoCount);
	DOREPLIFETIME(UMETWeaponAmmoComponent, OwningCharacter);
}
