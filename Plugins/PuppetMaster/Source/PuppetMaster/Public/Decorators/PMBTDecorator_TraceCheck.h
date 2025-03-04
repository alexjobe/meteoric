// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "PMBTDecorator_TraceCheck.generated.h"

/**
 * Returns true if there is a blocking hit
 */
UCLASS(MinimalAPI)
class UPMBTDecorator_TraceCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	// Z offset of querier
	UPROPERTY(EditAnywhere, Category = "Node")
	float QuerierHeightOffset;

	// Z offset of target
	UPROPERTY(EditAnywhere, Category = "Node")
	float TargetHeightOffset;

	UPROPERTY(EditAnywhere, Category = "Node")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	PUPPETMASTER_API UPMBTDecorator_TraceCheck();

	//~ Begin UBTDecorator_BlackboardBase interface
	PUPPETMASTER_API virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	PUPPETMASTER_API virtual FString GetStaticDescription() const override;
	//~ End UBTDecorator_BlackboardBase interface
};
