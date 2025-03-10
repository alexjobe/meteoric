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
	bool ClaimCoverSpot(class UPMCoverSpot* CoverSpot);
	void ReleaseCoverSpots();

	//~ Begin UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UActorComponent interface
	
#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	/* Tick is only used for drawing debug helpers - should only be enabled in development builds */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void DrawCoverDebug(const bool bShouldDraw);
#endif

private:
	UPROPERTY(Transient)
	TObjectPtr<UPMCoverSpot> OccupiedCoverSpot;

	UPROPERTY(Transient)
	TObjectPtr<UPMCoverSpot> ClaimedCoverSpot;

	UFUNCTION()
	void ClaimedCoverSpot_OnClaimChangedEvent(const AActor* Claimant);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void UnclaimCoverSpot();
};
