// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponSpreadComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponSpreadComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMETWeaponSpreadComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
	float TraceRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float MaxYawInDegrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spread")
	float MaxPitchInDegrees;

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	//~ End UActorComponent interface

	FTransform GetProjectileSpawnTransform() const;

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnWeaponUnequipped();

protected:
	FVector GetShotDirection(const FVector& InStartLocation) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(Transient)
	TObjectPtr<class AMETCharacter> OwningCharacter;
};
