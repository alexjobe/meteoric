// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "METWeapon.generated.h"

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

	AMETWeapon();

	void OnEquipped(ACharacter* InOwningCharacter);
	void Fire() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> CharacterFireMontage;
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetMesh() const { return Mesh; }


private:
	TObjectPtr<class ACharacter> OwningCharacter;

};
