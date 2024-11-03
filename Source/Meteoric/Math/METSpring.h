// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METSpring.generated.h"

USTRUCT(BlueprintType)
struct FMETSpring
{
	GENERATED_BODY()

	// Spring constant (k)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	float SpringConstant;

	// Mass of object (m)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	float Mass;

	// Automatically calculate DampingConstant to be critically damped: b == sqrt(4 * m * k)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	bool bCriticallyDamped;

	// Damping constant (b), related to friction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spring")
	float DampingConstant;
	
	FMETSpring();

	void Initialize();

	void Reset();

	void AddInstantaneousForce(float InForce, float InDeltaTime = 0.1f);

	float Update(float InDeltaTime);

	float GetCurrentDisplacement() const { return CurrentDisplacement; }

	FMETSpring& operator=(const FMETSpring& Other);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spring", meta = (AllowPrivateAccess = "true"))
	float DampingRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spring", meta = (AllowPrivateAccess = "true"))
	float DampedAngularFrequency;
	
	float ImpulseVelocity;
	float CurrentVelocity;
	float InitialDisplacement;
	float CurrentDisplacement;
	float LastDisplacement;
	float ElapsedTime;
	
	TQueue<float> DisplacementQueue;
	int DisplacementQueueSize;
	float DisplacementQueueTotal;
	bool bSettled;

	void UpdateSpring(float InDeltaTime);
	void UpdateDisplacementQueue();
};
