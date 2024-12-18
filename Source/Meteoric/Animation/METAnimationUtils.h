// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "METAnimationUtils.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API UMETAnimationUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Play Animation Montage on the provided mesh. Returns the length of the animation montage in seconds, or 0.f if failed to play. **/
	UFUNCTION(BlueprintCallable, Category = "Animation Utilities")
	static float PlayAnimMontage(USkeletalMeshComponent* Mesh, UAnimMontage* AnimMontage, const float InPlayRate = 1.f, const FName StartSectionName = NAME_None);
};
