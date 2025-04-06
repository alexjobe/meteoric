// Copyright Alex Jobe

#pragma once
#include "StateTreeConditionBase.h"

#include "PMSTCond_HasCover.generated.h"

USTRUCT()
struct FPMHasCoverInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Parameter)
	FVector TargetLocation = FVector::ZeroVector;
};

USTRUCT(meta = (DisplayName = "Has Cover", Category = "PuppetMaster|Cover"))
struct FPMSTCond_HasCover : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMHasCoverInstanceData;

	//~ Begin FStateTreeConditionCommonBase interface
	PMCOVERSYSTEM_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PMCOVERSYSTEM_API virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	//~ End FStateTreeConditionCommonBase interface
};