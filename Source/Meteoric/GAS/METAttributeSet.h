// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "METAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class METEORIC_API UMETAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//~ Begin UAttributeSet interface
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//~ End UAttributeSet interface

	/*
	 * Primary Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Health")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, HealthRegeneration)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Armor")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArmor, Category = "Armor")
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, MaxArmor)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Armor")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, ArmorPenetration)

	/*
	 * Meta Attributes
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData IncomingHealthDamage;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, IncomingHealthDamage)
	
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData IncomingArmorDamage;
	ATTRIBUTE_ACCESSORS(UMETAttributeSet, IncomingArmorDamage)

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& InOldData) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& InOldData) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& InOldData) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& InOldData) const;

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& InOldData) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& InOldData) const;
	
};
