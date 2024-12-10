// Copyright Alex Jobe


#include "METAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

void UMETAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, MaxArmor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMETAttributeSet, ArmorPiercing, COND_None, REPNOTIFY_Always);
}

void UMETAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	
	if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxArmor());
	}

	if (Attribute == GetArmorPiercingAttribute())
	{
		// Armor piercing is a percentage of damage
		NewValue = FMath::Clamp(NewValue, 0.0f, 100.f);
	}
}

void UMETAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetArmorAttribute())
	{
		SetArmor(FMath::Clamp(GetArmor(), 0.f, GetMaxArmor()));
	}

	if (Data.EvaluatedData.Attribute == GetArmorPiercingAttribute())
	{
		// Armor piercing is a percentage of damage
		SetArmorPiercing(FMath::Clamp(GetArmorPiercing(), 0.f, 100.f));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingHealthDamageAttribute())
	{
		// Consume incoming damage
		const float HealthDamageToApply = GetIncomingHealthDamage();
		SetIncomingHealthDamage(0.f);
		
		if (HealthDamageToApply > 0.f)
		{
			const float NewHealth = GetHealth() - HealthDamageToApply;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
		}
	}

	if (Data.EvaluatedData.Attribute == GetIncomingArmorDamageAttribute())
	{
		// Consume incoming damage
		const float ArmorDamageToApply = GetIncomingArmorDamage();
		SetIncomingArmorDamage(0.f);
		
		if (ArmorDamageToApply > 0.f)
		{
			const float NewArmor = GetArmor() - ArmorDamageToApply;
			SetArmor(FMath::Clamp(NewArmor, 0.f, GetMaxArmor()));
		}
	}
}

void UMETAttributeSet::OnRep_Health(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, Health, InOldData);
}

void UMETAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, MaxHealth, InOldData);
}

void UMETAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, HealthRegeneration, InOldData);
}

void UMETAttributeSet::OnRep_Armor(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, Armor, InOldData);
}

void UMETAttributeSet::OnRep_MaxArmor(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, MaxArmor, InOldData);
}

void UMETAttributeSet::OnRep_ArmorPiercing(const FGameplayAttributeData& InOldData) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMETAttributeSet, ArmorPiercing, InOldData);
}
