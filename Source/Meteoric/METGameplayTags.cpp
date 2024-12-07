// Copyright Alex Jobe

#include "METGameplayTags.h"

namespace METGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_FireWeapon, "InputTag.FireWeapon", "Fire weapon input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AimDownSights, "InputTag.AimDownSights", "Aim down sights input");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Health, "Attribute.Health", "Health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxHealth, "Attribute.MaxHealth", "Max health attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_HealthRegeneration, "Attribute.HealthRegeneration", "Health regeneration attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_Armor, "Attribute.Armor", "Armor attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_MaxArmor, "Attribute.MaxArmor", "Max armor attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_ArmorPenetration, "Attribute.ArmorPenetration", "Armor penetration attribute");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attribute_IncomingDamage, "Attribute.IncomingDamage", "Incoming damage meta attribute");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_SingleShot, "FiringMode.SingleShot", "Single shot weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_Automatic, "FiringMode.Automatic", "Automatic weapon");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "Dead state");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Damage, "Damage", "Damage");
}
