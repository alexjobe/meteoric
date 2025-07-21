// Copyright Alex Jobe


#include "METAmmoDataAsset.h"

UParticleSystem* FMETAmmoDamageConfig::GetVFX(const EPhysicalSurface& SurfaceType) const
{
	if (const auto Value = FXSettings.SurfaceToVFX.Find(SurfaceType))
	{
		return *Value;
	}
	return nullptr;
}

USoundCue* FMETAmmoDamageConfig::GetSFX(const EPhysicalSurface& SurfaceType) const
{
	if (const auto Value = FXSettings.SurfaceToSFX.Find(SurfaceType))
	{
		return *Value;
	}
	return nullptr;
}
