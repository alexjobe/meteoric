// Copyright Alex Jobe

#pragma once

#include "StateTreeTaskBase.h"

#include "PMSTTask_SetFocus.generated.h"

/**
* Set focus to provided target actor
*/
USTRUCT()
struct FPMSetFocusTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<AActor> TargetActor;
};

USTRUCT(meta = (DisplayName = "Set Focus", Category = "PuppetMaster"))
struct FPMSTTask_SetFocus : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()
	
	using FInstanceDataType = FPMSetFocusTaskInstanceData;

	//~ Begin FStateTreeTaskCommonBase interface
	PUPPETMASTER_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PUPPETMASTER_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	PUPPETMASTER_API virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	//~ End FStateTreeTaskCommonBase interface
};