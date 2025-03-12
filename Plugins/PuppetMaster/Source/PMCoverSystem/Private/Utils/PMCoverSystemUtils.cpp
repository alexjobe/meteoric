// Copyright Alex Jobe


#include "Utils/PMCoverSystemUtils.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

FVector UPMCoverSystemUtils::GetKeyLocation(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key)
{
	FVector KeyLocation = FVector::ZeroVector;
	if (!ensure(BlackboardComp)) return KeyLocation;

	if (Key.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Object>(Key.GetSelectedKeyID());
		if (const AActor* KeyActor = Cast<AActor>(KeyValue))
		{
			KeyLocation = KeyActor->GetActorLocation();
		}
	}
	else if (Key.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyLocation = BlackboardComp->GetValue<UBlackboardKeyType_Vector>(Key.GetSelectedKeyID());
	}
	
	return KeyLocation;
}

float UPMCoverSystemUtils::GetKeyFloatValue(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key)
{
	float KeyValue = 0.0f;
	if (!ensure(BlackboardComp)) return KeyValue;

	if (Key.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Float>(Key.GetSelectedKeyID());
	}
	
	return KeyValue;
}
