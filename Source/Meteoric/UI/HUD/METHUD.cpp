// Copyright Alex Jobe


#include "METHUD.h"

#include "Blueprint/UserWidget.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"
#include "Meteoric/UI/Widget/METOverlayWidget.h"

void AMETHUD::Initialize(UMETAbilitySystemComponent* InASC)
{
	if (!ensure(InASC)) return;

	OverlayWidget = CreateWidget<UMETOverlayWidget>(GetOwningPlayerController(), OverlayWidgetClass, FName("OverlayWidget"));
	
	AbilitySystemComponent = InASC;
	const UMETAttributeSet* AttributeSet = Cast<UMETAttributeSet>(AbilitySystemComponent->GetAttributeSet(UMETAttributeSet::StaticClass()));
	
	if (!ensure(AttributeSet)) return;

	SetMaxHealth(AttributeSet->GetMaxHealth());
	SetHealth(AttributeSet->GetHealth());
	SetMaxArmor(AttributeSet->GetMaxArmor());
	SetArmor(AttributeSet->GetArmor());
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetArmorAttribute()).AddUObject(this, &ThisClass::ArmorChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxArmorAttribute()).AddUObject(this, &ThisClass::MaxArmorChanged);

	if (OverlayWidget)
	{
		OverlayWidget->AddToViewport();
	}
}

void AMETHUD::SetHealth(const float InNewValue) const
{
	if (OverlayWidget)
	{
		OverlayWidget->SetHealth(InNewValue);
	}
}

void AMETHUD::SetMaxHealth(const float InNewValue) const
{
	if (OverlayWidget)
	{
		OverlayWidget->SetMaxHealth(InNewValue);
	}
}

void AMETHUD::SetArmor(const float InNewValue) const
{
	if (OverlayWidget)
	{
		OverlayWidget->SetArmor(InNewValue);
	}
}

void AMETHUD::SetMaxArmor(const float InNewValue) const
{
	if (OverlayWidget)
	{
		OverlayWidget->SetMaxArmor(InNewValue);
	}
}

void AMETHUD::HealthChanged(const FOnAttributeChangeData& Data) const
{
	SetHealth(Data.NewValue);
}

void AMETHUD::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	SetMaxHealth(Data.NewValue);
}

void AMETHUD::ArmorChanged(const FOnAttributeChangeData& Data) const
{
	SetArmor(Data.NewValue);
}

void AMETHUD::MaxArmorChanged(const FOnAttributeChangeData& Data) const
{
	SetMaxArmor(Data.NewValue);
}
