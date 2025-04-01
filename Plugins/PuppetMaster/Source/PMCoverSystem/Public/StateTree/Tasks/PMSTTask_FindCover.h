// Copyright Alex Jobe

#pragma once

#include "PMCoverSystemTypes.h"
#include "StateTreeTaskBase.h"

#include "PMSTTask_FindCover.generated.h"

USTRUCT()
struct FPMFindCoverTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Output)
	FVector FoundCoverLocation = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = Parameter)
	EPMCoverSearchMode SearchMode = EPMCoverSearchMode::LeastDistanceToQuerier;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector SearchCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float SearchRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float MinDistanceToTarget = 500.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	float MaxDistanceToTarget = 2000.f;

	UPROPERTY(EditAnywhere, Category = Parameter)
	bool bTestCoverSpotNavigable = true;
};

USTRUCT(meta = (DisplayName = "Find Cover", Category = "PuppetMaster|Cover"))
struct FPMSTTask_FindCover : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMFindCoverTaskInstanceData;

	FPMSTTask_FindCover();

	//~ Begin FStateTreeTaskCommonBase interface
	PMCOVERSYSTEM_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PMCOVERSYSTEM_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~ End FStateTreeTaskCommonBase interface
};