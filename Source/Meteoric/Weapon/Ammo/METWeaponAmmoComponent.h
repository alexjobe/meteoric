// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponAmmoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponAmmoComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxCount;
	
	UMETWeaponAmmoComponent();

	virtual void BeginPlay() override;
	
	int32 Reload();

	int32 GetAmmoCount() const { return AmmoCount; }

	// Try to consume ammo, and return true if successful
	bool TryConsumeAmmo(const int32 InConsumeCount = 1);

	void OnWeaponEquipped(ACharacter* const InOwningCharacter);
	void OnWeaponUnequipped();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UMETAmmoDataAsset> CurrentAmmoType;
	
	int32 AmmoCount;

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;
};
