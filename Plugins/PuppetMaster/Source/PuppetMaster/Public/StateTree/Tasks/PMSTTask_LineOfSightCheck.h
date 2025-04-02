// Copyright Alex Jobe

#pragma once
#include "StateTreeTaskBase.h"

#include "PMSTTask_LineOfSightCheck.generated.h"

USTRUCT()
struct FPMLineOfSightCheckInstanceData
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

USTRUCT(meta = (DisplayName = "Line of Sight Check", Category = "PuppetMaster"))
struct FPMSTTask_LineOfSightCheck : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FPMLineOfSightCheckInstanceData;

	//~ Begin FStateTreeTaskCommonBase interface
	PUPPETMASTER_API virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	PUPPETMASTER_API virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	PUPPETMASTER_API virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	//~ End FStateTreeTaskCommonBase interface
};