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
	AMETWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equip")
	FName ParentAttachmentSocket;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class USkeletalMeshComponent> Mesh;
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
