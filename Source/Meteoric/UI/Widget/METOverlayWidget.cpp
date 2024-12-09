// Copyright Alex Jobe


#include "METOverlayWidget.h"

UMETOverlayWidget::UMETOverlayWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Health(0.f)
	, MaxHealth(0.f)
	, Armor(0.f)
	, MaxArmor(0.f)
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
