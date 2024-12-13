// Copyright Alex Jobe


#include "METHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"
#include "Meteoric/UI/Widget/METOverlayWidget.h"
#include "Meteoric/Weapon/Ammo/METAmmoManager.h"

void AMETHUD::Initialize(UMETAbilitySystemComponent* InASC)
{
	if (!ensure(InASC)) return;

	ACharacter* OwningCharacter = Cast<ACharacter>(InASC->GetAvatarActor());
	UMETAmmoManager* AmmoManager = OwningCharacter ? OwningCharacter->FindComponentByClass<UMETAmmoManager>() : nullptr;
	if (ensure(AmmoManager))
	{
		AmmoManager->OnWeaponAmmoChanged().AddUniqueDynamic(this, &ThisClass::AMETHUD::AmmoManager_WeaponAmmoChanged);
		AmmoManager->OnReserveAmmoChanged().AddUniqueDynamic(this, &ThisClass::AMETHUD::AmmoManager_ReserveAmmoChanged);
	}

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

void AMETHUD::AmmoManager_WeaponAmmoChanged(int32 AmmoCount, int32 MaxAmmo)
{
	if (OverlayWidget)
	{
		OverlayWidget->SetWeaponAmmo(AmmoCount, MaxAmmo);
	}
}

void AMETHUD::AmmoManager_ReserveAmmoChanged(int32 AmmoCount, int32 MaxAmmo)
{
	if (OverlayWidget)
	{
		OverlayWidget->SetReserveAmmo(AmmoCount, MaxAmmo);
	}
}
