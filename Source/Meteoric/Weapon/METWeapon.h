// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "METWeapon.generated.h"

UENUM(BlueprintType)
enum EWeaponFiringMode
{
	SingleShot,
	Automatic
};

UCLASS()
class METEORIC_API AMETWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
	FName ParentAttachmentSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float SightCameraOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ADS")
	float AimDownSightsSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing")
	TEnumAsByte<EWeaponFiringMode> FiringMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firing")
	float FiringRate;

	AMETWeapon();

	void OnEquipped(ACharacter* InOwningCharacter);
	void OnFireActionStarted();
	void OnFireActionHeld();

	UAnimSequence* GetCharacterIdleWeaponAnim() const { return CharacterIdleWeaponAnim; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UMETRecoilComponent> RecoilComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> CharacterFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> CharacterIdleWeaponAnim;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimSequence> WeaponFireAnim;
	
	virtual void BeginPlay() override;

	void Fire();

public:	
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetMesh() const { return Mesh; }

private:
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwningCharacter;

	float LastTimeFired;
};
