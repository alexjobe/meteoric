﻿// Copyright Alex Jobe

#pragma once

#include "StateTreeTaskBase.h"

#include "PMSTTask_GetTargetActor.generated.h"

USTRUCT()
struct FPMGetTargetActorTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Output)
	TObjectPtr<AActor> TargetActor;
};

/**
* Returns the TargetActor from PMPuppetComponent
*/
USTRUCT(meta = (DisplayName = "Get Target Actor", Category = "PuppetMaster"))
struct FPMSTTask_GetTargetActor : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FPMGetTargetActorTaskInstanceData;

	FPMSTTask_GetTargetActor();

	//~ Begin FStateTreeTaskCommonBase interface
	PUPPETMASTER_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PUPPETMASTER_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~ End FStateTreeTaskCommonBase interface
};
