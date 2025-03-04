// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "PMBTDecorator_DistanceCheck.generated.h"

/**
 * Returns true if target distance falls within provided range
 */
UCLASS(MinimalAPI)
class UPMBTDecorator_DistanceCheck : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Node")
	float MinimumDistance;
	
	UPROPERTY(EditAnywhere, Category = "Node")
	float MaximumDistance;
	
	PUPPETMASTER_API UPMBTDecorator_DistanceCheck();

	//~ Begin UBTDecorator_BlackboardBase interface
	PUPPETMASTER_API virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	PUPPETMASTER_API virtual FString GetStaticDescription() const override;
	//~ End UBTDecorator_BlackboardBase interface

	static float DistanceSquared(const FVector& LocationA, const FVector& LocationB);
	static bool IsDistanceWithinRange(const FVector& LocationA, const FVector& LocationB, float MinDistance, float MaxDistance);
};
