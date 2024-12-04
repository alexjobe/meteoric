// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METProjectileWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETProjectileWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<class AMETProjectile> ProjectileClass;

public:	
	UMETProjectileWeaponComponent();

	void FireProjectile(const FTransform& InSpawnTransform, APawn* InOwner) const;
};
