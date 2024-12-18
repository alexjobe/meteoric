// Copyright Alex Jobe


#include "METAnimationUtils.h"

float UMETAnimationUtils::PlayAnimMontage(USkeletalMeshComponent* Mesh, UAnimMontage* AnimMontage, const float InPlayRate, const FName StartSectionName)
{
	UAnimInstance* AnimInstance = (Mesh)? Mesh->GetAnimInstance() : nullptr; 
	if( AnimMontage && AnimInstance )
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}	

	return 0.f;
}
