// Copyright Alex Jobe


#include "Decorators/PMBTDecorator_DistanceCheck.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UPMBTDecorator_DistanceCheck::UPMBTDecorator_DistanceCheck()
{
	NodeName = "Distance Check";
	
	// Accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTDecorator_DistanceCheck, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTDecorator_DistanceCheck, BlackboardKey));
}

bool UPMBTDecorator_DistanceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* MyPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ensure(MyPawn)) return false;
	
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (!ensure(MyBlackboard)) return false;
	
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
		if (const AActor* TargetActor = Cast<AActor>(KeyValue))
		{
			return IsDistanceWithinRange(MyPawn->GetActorLocation(), TargetActor->GetActorLocation(), MinimumDistance, MaximumDistance);
		}
	}

	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
		return IsDistanceWithinRange(MyPawn->GetActorLocation(), TargetLocation, MinimumDistance, MaximumDistance);
	}

	return false;
}

FString UPMBTDecorator_DistanceCheck::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}

float UPMBTDecorator_DistanceCheck::DistanceSquared(const FVector& LocationA, const FVector& LocationB)
{
	return FMath::Square(LocationA.X - LocationB.X) + FMath::Square(LocationA.Y - LocationB.Y) + FMath::Square(LocationA.Z - LocationB.Z);
}

bool UPMBTDecorator_DistanceCheck::IsDistanceWithinRange(const FVector& LocationA, const FVector& LocationB, const float MinDistance, const float MaxDistance)
{
	const float DistSquared = DistanceSquared(LocationA, LocationB);
	return DistSquared >= FMath::Square(MinDistance) && DistSquared <= FMath::Square(MaxDistance);
}
