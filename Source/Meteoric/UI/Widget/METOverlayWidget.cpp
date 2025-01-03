// Copyright Alex Jobe


#include "METOverlayWidget.h"

UMETOverlayWidget::UMETOverlayWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Health(0.f)
	, MaxHealth(0.f)
	, Armor(0.f)
	, MaxArmor(0.f)
	, WeaponAmmoCount(0)
	, WeaponMaxAmmoCount(0)
	, ReserveAmmoCount(0)
	, ReserveMaxAmmoCount(0)
{
}

void UMETOverlayWidget::SetHealth(const float InNewValue)
{
	Health = InNewValue;
	OnHealthChanged.Broadcast(InNewValue);
}

void UMETOverlayWidget::SetMaxHealth(const float InNewValue)
{
	MaxHealth = InNewValue;
	OnMaxHealthChanged.Broadcast(InNewValue);
}

void UMETOverlayWidget::SetArmor(const float InNewValue)
{
	Armor = InNewValue;
	OnArmorChanged.Broadcast(InNewValue);
}

void UMETOverlayWidget::SetMaxArmor(const float InNewValue)
{
	MaxArmor = InNewValue;
	OnMaxArmorChanged.Broadcast(InNewValue);
}

void UMETOverlayWidget::SetWeaponAmmoCount(const int32 InAmmoCount, const int32 InMaxAmmo)
{
	WeaponAmmoCount = InAmmoCount;
	WeaponMaxAmmoCount = InMaxAmmo;
	OnWeaponAmmoCountChanged.Broadcast(WeaponAmmoCount, WeaponMaxAmmoCount);
}

void UMETOverlayWidget::SetReserveAmmoCount(const int32 InAmmoCount, const int32 InMaxAmmo)
{
	ReserveAmmoCount = InAmmoCount;
	ReserveMaxAmmoCount = InMaxAmmo;
	OnReserveAmmoCountChanged.Broadcast(ReserveAmmoCount, ReserveMaxAmmoCount);
}

void UMETOverlayWidget::SetEquippedAmmoType(const UMETAmmoDataAsset* const InAmmoType) const
{
	OnEquippedAmmoTypeChanged.Broadcast(InAmmoType);
}
