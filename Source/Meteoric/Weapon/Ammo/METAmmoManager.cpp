// Copyright Alex Jobe


#include "METAmmoManager.h"

#include "GameplayEffect.h"
#include "METAmmoDataAsset.h"
#include "GameFramework/Character.h"
#include "Meteoric/GAS/METAbilitySystemUtils.h"

UMETAmmoManager::UMETAmmoManager()
	: ReserveMaxAmmo(1000)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETAmmoManager::OnPossessed() const
{
	WeaponAmmoTypeChangedEvent.Broadcast(EquippedWeaponAmmoType, 0, 0);
}

int32 UMETAmmoManager::GetReserveAmmoCount(UMETAmmoDataAsset* const InType)
{
	if (!InType || !ensure(InType->WeaponTag.IsValid())) return 0;
	auto& [AmmoTypeToCount] = WeaponToReserveAmmo.FindOrAdd(InType->WeaponTag);
	return AmmoTypeToCount.FindOrAdd(InType);
}

int32 UMETAmmoManager::AddReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InAmmoCount)
{
	if (!ensure(InType) || !ensure(InType->WeaponTag.IsValid()) || !GetOwner()->HasAuthority()) return 0;
	auto& [AmmoTypeToCount] = WeaponToReserveAmmo.FindOrAdd(InType->WeaponTag);
	int32& AmmoCount = AmmoTypeToCount.FindOrAdd(InType);
	
	if (InAmmoCount > 0)
	{
		AmmoCount = FMath::Min(AmmoCount + InAmmoCount, ReserveMaxAmmo);
		ReserveAmmoChangedEvent.Broadcast(InType, AmmoCount, ReserveMaxAmmo);
		Client_SetReserveAmmo(InType, AmmoCount, ReserveMaxAmmo);
	}
	return AmmoCount;
}

int32 UMETAmmoManager::TryConsumeReserveAmmo(UMETAmmoDataAsset* const InType, const int32 InConsumeCount)
{
	if (!ensure(InType) || !ensure(InType->WeaponTag.IsValid()) || !GetOwner()->HasAuthority()) return 0;
	auto& [AmmoTypeToCount] = WeaponToReserveAmmo.FindOrAdd(InType->WeaponTag);
	int32& AmmoCount = AmmoTypeToCount.FindOrAdd(InType);
	
	const int32 OriginalCount = AmmoCount;
	const int32 ConsumeCount = InConsumeCount > 0 ? InConsumeCount : 0;
	AmmoCount = FMath::Max(0, OriginalCount - ConsumeCount);

	ReserveAmmoChangedEvent.Broadcast(InType, AmmoCount, ReserveMaxAmmo);
	Client_SetReserveAmmo(InType, AmmoCount, ReserveMaxAmmo);

	return FMath::Min(OriginalCount, ConsumeCount);
}

void UMETAmmoManager::WeaponAmmoChanged(const int32 InAmmoCount, const int32 InMaxAmmo) const
{
	WeaponAmmoChangedEvent.Broadcast(EquippedWeaponAmmoType, InAmmoCount, InMaxAmmo);
	if (GetOwner()->HasAuthority())
	{
		Client_WeaponAmmoChanged(InAmmoCount, InMaxAmmo);
	}
}

void UMETAmmoManager::WeaponAmmoTypeChanged(const AActor* const InWeapon, UMETAmmoDataAsset* const InType)
{
	if (EquippedWeaponAmmoType == InType) return;
	EquippedWeaponAmmoType = InType;
	const int32 AmmoCount = GetReserveAmmoCount(InType);
	WeaponAmmoTypeChangedEvent.Broadcast(InType, AmmoCount, ReserveMaxAmmo);
	if (GetOwner()->HasAuthority())
	{
		RemoveActiveAmmoEffectFromOwner();
		ApplyAmmoEffectToOwner(InWeapon);
		Client_WeaponAmmoTypeChanged(InWeapon, InType);
	}
}

void UMETAmmoManager::ApplyAmmoEffectToOwner(const AActor* const InSource)
{
	const ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!ensure(OwningCharacter)) return;
	
	if (!EquippedWeaponAmmoType || !EquippedWeaponAmmoType->EquippedEffectClass) return;

	if (OwningCharacter->HasAuthority())
	{
		ActiveAmmoEffectHandle = UMETAbilitySystemUtils::ApplyEffectClassToActor(
			OwningCharacter,
			InSource,
			EquippedWeaponAmmoType->EquippedEffectClass,
			1
		);
	}
}

void UMETAmmoManager::RemoveActiveAmmoEffectFromOwner()
{
	const ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());
	if (!ensure(OwningCharacter)) return;
	
	if (OwningCharacter->HasAuthority() && ActiveAmmoEffectHandle.IsSet())
	{
		UMETAbilitySystemUtils::RemoveEffectFromActor(OwningCharacter, ActiveAmmoEffectHandle.GetValue());
		ActiveAmmoEffectHandle.Reset();
	}
}

void UMETAmmoManager::Client_WeaponAmmoTypeChanged_Implementation(const AActor* const InWeapon, UMETAmmoDataAsset* InType)
{
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		WeaponAmmoTypeChanged(InWeapon, InType);
	}
}

void UMETAmmoManager::Client_WeaponAmmoChanged_Implementation(const int32 InAmmoCount, const int32 InMaxAmmo) const
{
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		WeaponAmmoChanged(InAmmoCount, InMaxAmmo);
	}
}

void UMETAmmoManager::Client_SetReserveAmmo_Implementation(UMETAmmoDataAsset* InType, const int32 InAmmoCount, const int32 InMaxAmmo)
{
	if (!ensure(InType) || !ensure(InType->WeaponTag.IsValid())) return;
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		auto& [AmmoTypeToCount] = WeaponToReserveAmmo.FindOrAdd(InType->WeaponTag);
		int32& AmmoCount = AmmoTypeToCount.FindOrAdd(InType);
		AmmoCount = InAmmoCount;
		ReserveMaxAmmo = InMaxAmmo;
		ReserveAmmoChangedEvent.Broadcast(InType, AmmoCount, ReserveMaxAmmo);
	}
}
