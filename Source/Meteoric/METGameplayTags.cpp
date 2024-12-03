// Copyright Alex Jobe

#include "METGameplayTags.h"

namespace METGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_FireWeapon, "InputTag.FireWeapon", "Fire weapon input");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_AimDownSights, "InputTag.AimDownSights", "Aim down sights input");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_SingleShot, "FiringMode.SingleShot", "Single shot weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FiringMode_Automatic, "FiringMode.Automatic", "Automatic weapon");
}
