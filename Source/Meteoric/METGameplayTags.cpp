// Copyright Alex Jobe

#include "METGameplayTags.h"

namespace METGameplayTags
{
	/* Input tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_FireWeapon, "InputTag.FireWeapon", "Fire weapon input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AimDownSights, "InputTag.AimDownSights", "Aim down sights input");

	/* Attribute tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "Health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxHealth, "Attribute.MaxHealth", "Max health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthRegeneration, "Attribute.HealthRegeneration", "Health regeneration attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Armor, "Attribute.Armor", "Armor attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxArmor, "Attribute.MaxArmor", "Max armor attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ArmorPiercing, "Attribute.ArmorPiercing", "Armor piercing attribute");

	/* Meta attribute tags */
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_IncomingHealthDamage, "Attribute.IncomingHealthDamage", "Incoming health damage meta attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_IncomingArmorDamage, "Attribute.IncomingArmorDamage", "Incoming armor damage meta attribute");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_SingleShot, "FiringMode.SingleShot", "Single shot weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_Automatic, "FiringMode.Automatic", "Automatic weapon");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "Dead state");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Damage");
}
