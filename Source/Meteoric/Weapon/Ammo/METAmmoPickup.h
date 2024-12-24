// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "METAmmoPickup.generated.h"

UCLASS()
class METEORIC_API AMETAmmoPickup : public AActor
{
	GENERATED_BODY()
	
public:
	AMETAmmoPickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoCount;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
	TObjectPtr<class UMETAmmoDataAsset> AmmoType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USphereComponent> CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETInteractableComponent> InteractableComponent;

private:
	UFUNCTION()
	void InteractableComponent_OnInteractEvent(AActor* const InSourceActor);
};
