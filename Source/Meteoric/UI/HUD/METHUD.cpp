// Copyright Alex Jobe


#include "METHUD.h"

#include "Meteoric/GAS/METAbilitySystemComponent.h"
#include "Meteoric/GAS/METAttributeSet.h"

void AMETHUD::SetAbilitySystemComponent(UMETAbilitySystemComponent* InASC)
{
	if (!ensure(InASC)) return;
	
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
}

void AMETHUD::HealthChanged(const FOnAttributeChangeData& Data)
{
	SetHealth(Data.NewValue);
}

void AMETHUD::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	SetMaxHealth(Data.NewValue);
}

void AMETHUD::ArmorChanged(const FOnAttributeChangeData& Data)
{
	SetArmor(Data.NewValue);
}

void AMETHUD::MaxArmorChanged(const FOnAttributeChangeData& Data)
{
	SetMaxArmor(Data.NewValue);
}
