// Copyright Alex Jobe


#include "Decorators/PMBTDecorator_TraceCheck.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UPMBTDecorator_TraceCheck::UPMBTDecorator_TraceCheck()
	: QuerierHeightOffset(0.0f)
	, TargetHeightOffset(0.f)
	, TraceChannel(ECC_Visibility)
{
	NodeName = "Trace Check";
	
	// Accept only actors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTDecorator_TraceCheck, BlackboardKey), APawn::StaticClass());
}

bool UPMBTDecorator_TraceCheck::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	const APawn* MyPawn = AIController ? AIController->GetPawn() : nullptr;
	if (!ensure(MyPawn)) return false;

	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	UObject* KeyValue = MyBlackboard ? MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID()) : nullptr;
	const APawn* TargetPawn = Cast<APawn>(KeyValue);
	if (!ensure(TargetPawn)) return false;
	
	const FVector TraceStart = MyPawn->GetNavAgentLocation() + FVector(0, 0, QuerierHeightOffset);
	const FVector TraceEnd = TargetPawn->GetNavAgentLocation() + FVector(0, 0, TargetHeightOffset);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 2.f, 0, 1.f);

	return GetWorld()->LineTraceTestByChannel(TraceStart, TraceEnd, TraceChannel);;
}

FString UPMBTDecorator_TraceCheck::GetStaticDescription() const
{
	FString KeyDesc("invalid");
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}
