// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METRecoilComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETRecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMETRecoilComponent();

	void OnWeaponEquipped(ACharacter* InOwningCharacter);
	void OnFireActionStarted();
	void OnFireActionHeld();
	void OnWeaponFired();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recoil")
	UCurveFloat* RecoilCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float VerticalRecoilMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float HorizontalRecoilMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil")
	float RecoilNoise;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	float FireActionStartTime;
	FVector2d CurrentRecoilCurvePos;
	FVector2d LastRecoilCurvePos;
};
