// Copyright Alex Jobe


#include "METExecCalc_Damage.h"

#include "Meteoric/METGameplayTags.h"
#include "Meteoric/GAS/METAttributeSet.h"

// Declare the attributes to capture and define how we want to capture them from the Source and Target.
struct METDamageStatics
{
	// Prevent direct construction of AuraDamageStatics, so it's mandatory to use DamageStatics() method
	// DamageStatics() is friend so it can call constructor
	friend static METDamageStatics& DamageStatics();
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);

private:
	METDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMETAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMETAttributeSet, ArmorPenetration, Source, true);
	}
};

static METDamageStatics& DamageStatics()
{
	static METDamageStatics DamageStatics;
	return DamageStatics;
}

UMETExecCalc_Damage::UMETExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
}

void UMETExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// Gather tags from source and target
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float TotalDamage = 0.f;
	TotalDamage += FMath::Max<float>(0.f, EffectSpec.GetSetByCallerMagnitude(METGameplayTags::Damage));

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(0.f, Armor);
	
	float ArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, ArmorPenetration);
	ArmorPenetration = FMath::Max<float>(0.f, ArmorPenetration);
	
	float PenetrationDamage = FMath::Min(ArmorPenetration, TotalDamage);
	float HealthDamage = PenetrationDamage;
	float BlockableDamage =  FMath::Max(0.f, TotalDamage - PenetrationDamage);

	HealthDamage += FMath::Max<float>(0.f, BlockableDamage - Armor);
	const float ArmorDamage = FMath::Max<float>(0.f, (TotalDamage - HealthDamage) + PenetrationDamage * 0.5f);
	
	const FGameplayModifierEvaluatedData EvaluatedHealthDamage(UMETAttributeSet::GetIncomingHealthDamageAttribute(), EGameplayModOp::Additive, HealthDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedHealthDamage);
	
	const FGameplayModifierEvaluatedData EvaluatedArmorDamage(UMETAttributeSet::GetIncomingArmorDamageAttribute(), EGameplayModOp::Additive, ArmorDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedArmorDamage);
}
