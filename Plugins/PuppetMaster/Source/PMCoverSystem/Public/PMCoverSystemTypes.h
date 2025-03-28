// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "PMCoverSystemTypes.generated.h"

UENUM(BlueprintType)
enum class EPMCoverSearchMode : uint8
{
	LeastDistanceToQuerier,
	GreatestDistanceToQuerier,
	LeastDistanceToTarget,
	GreatestDistanceToTarget
};

USTRUCT()
struct FPMFindCoverParams
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	AActor* ContextActor;

	EPMCoverSearchMode SearchMode;
	FVector TargetLocation;
	FVector SearchCenter;
	float SearchRadius;
	float MinDistanceToTarget;
	float MaxDistanceToTarget;
	bool bTestCoverSpotNavigable;

	FPMFindCoverParams()
		: ContextActor(nullptr)
		, SearchMode(EPMCoverSearchMode::LeastDistanceToQuerier)
		, TargetLocation(FVector::ZeroVector)
		, SearchCenter(FVector::ZeroVector)
		, SearchRadius(1000.f)
		, MinDistanceToTarget(500.f)
		, MaxDistanceToTarget(2000.f)
		, bTestCoverSpotNavigable(true)
	{}
};