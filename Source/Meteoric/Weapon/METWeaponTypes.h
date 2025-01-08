// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METWeaponTypes.generated.h"

UENUM(BlueprintType)
enum EWeaponFiringMode
{
	SingleShot,
	Automatic
};

USTRUCT(BlueprintType)
struct FMETWeaponAnimationSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimMontage> CharacterFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimSequence> CharacterIdleWeaponAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimMontage> CharacterEquipWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimMontage> CharacterReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> WeaponFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UAnimMontage> WeaponReloadMontage;
};

USTRUCT(BlueprintType)
struct FMETWeaponFXSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> UnequipSound;
};