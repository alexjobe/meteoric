// Copyright Alex Jobe

#pragma once
#include "StateTreeConditionBase.h"

#include "PMSTCond_HasLineOfSight.generated.h"

USTRUCT()
struct FPMHasLineOfSightInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<class AAIController> AIController;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, Category = Parameter)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// Z offset of querier
	UPROPERTY(EditAnywhere, Category = Parameter)
	float QuerierHeightOffset = 0.f;

	// Z offset of target
	UPROPERTY(EditAnywhere, Category = Parameter)
	float TargetHeightOffset = 0.f;
};

/**
*
*/
USTRUCT(meta = (DisplayName = "Has Line of Sight", Category = "PuppetMaster"))
struct FPMSTCond_HasLineOfSight : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMHasLineOfSightInstanceData;

	//~ Begin FStateTreeConditionCommonBase interface
	PUPPETMASTER_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PUPPETMASTER_API virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
	//~ End FStateTreeConditionCommonBase interface
};