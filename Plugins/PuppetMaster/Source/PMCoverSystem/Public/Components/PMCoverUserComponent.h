// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PMCoverUserComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PMCOVERSYSTEM_API UPMCoverUserComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPMCoverUserComponent();

	void InitializeCoverUser(UPrimitiveComponent* OverlappedComponent);
	void ReleaseCoverSpot() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<class UPMCoverSpot> ClaimedCoverSpot;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
