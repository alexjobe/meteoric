// Copyright Alex Jobe

#pragma once
#include "StateTreeTaskBase.h"

#include "PMSTTask_CoverCheck.generated.h"

USTRUCT()
struct FPMCoverCheckInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector TargetLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "Cover Check", Category = "PuppetMaster|Cover"))
struct FPMSTTask_CoverCheck : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMCoverCheckInstanceData;

	//~ Begin FStateTreeTaskCommonBase interface
	PMCOVERSYSTEM_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PMCOVERSYSTEM_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	PMCOVERSYSTEM_API virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	//~ End FStateTreeTaskCommonBase interface
};