// Copyright Alex Jobe


#include "Tasks/PMBTTask_FindCover.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Components/PMCoverComponent.h"
#include "Components/PMCoverSpot.h"
#include "Components/PMCoverUserComponent.h"
#include "Interface/PMCoverInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Logging/PMCoverSystemLog.h"


UPMBTTask_FindCover::UPMBTTask_FindCover()
	: SearchRadius(1000.f)
	, bTestCoverSpotNavigable(true)
{
	NodeName = "FindCover";
	
	// Accept only actors and vectors
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, TargetKey), AActor::StaticClass());
	TargetKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, TargetKey));

	SearchCenterKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, SearchCenterKey), AActor::StaticClass());
	SearchCenterKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, SearchCenterKey));

	// Accept only floats
	MinDistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MinDistanceToTargetKey));
	MaxDistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MaxDistanceToTargetKey));

	// Accept only vector
	MoveToLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UPMBTTask_FindCover, MoveToLocationKey));
}

void UPMBTTask_FindCover::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetKey.ResolveSelectedKey(*BBAsset);
		SearchCenterKey.ResolveSelectedKey(*BBAsset);
		MinDistanceToTargetKey.ResolveSelectedKey(*BBAsset);
		MaxDistanceToTargetKey.ResolveSelectedKey(*BBAsset);
		MoveToLocationKey.ResolveSelectedKey(*BBAsset);
	}
	else
	{
		UE_LOG(LogCoverSystem, Error, TEXT("Can't initialize task: %s, make sure that behavior tree specifies blackboard asset!"), *GetName());
	}
}

EBTNodeResult::Type UPMBTTask_FindCover::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* Controller = OwnerComp.GetAIOwner();
	const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UPMCoverUserComponent* CoverUserComponent = Pawn ? Pawn->FindComponentByClass<UPMCoverUserComponent>() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	if (!ensure(CoverUserComponent) || !ensure(BlackboardComp))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMBTTask_FindCover::ExecuteTask -- Owner must have BlackboardComponent and CoverUserComponent! %s"),
			*OwnerComp.GetName())
		return EBTNodeResult::Failed;
	}

	const FVector TargetLocation = GetKeyLocation(BlackboardComp, TargetKey);
	const FVector SearchCenter = GetKeyLocation(BlackboardComp, SearchCenterKey);
	const float MinDistanceToTarget = GetKeyFloatValue(BlackboardComp, MinDistanceToTargetKey);
	const float MaxDistanceToTarget = GetKeyFloatValue(BlackboardComp, MaxDistanceToTargetKey);

	// Find all cover actors (IPMCoverInterface) in search radius
	TArray<AActor*> FoundCoverActors = FindCoverActors(Pawn, SearchCenter, SearchRadius);
	if (FoundCoverActors.Num() == 0) return EBTNodeResult::Failed;

	// Filter out cover actors that do not fall within ideal distance to target
	TArray<AActor*> FilteredCoverActors = FilterCoverActors(FoundCoverActors, TargetLocation, MinDistanceToTarget, MaxDistanceToTarget);
	if (FilteredCoverActors.Num() == 0) return EBTNodeResult::Failed;

	// Sort cover actors by search mode
	SortCoverActors(FilteredCoverActors, TargetLocation, Pawn->GetActorLocation(), SearchMode);

	// Once cover actors are sorted, iterate until we find one with a valid cover spot
	if (UPMCoverSpot* BestCoverSpot = GetBestCoverSpot(FilteredCoverActors, TargetLocation, Pawn, bTestCoverSpotNavigable))
	{
		if (CoverUserComponent->ReserveCoverSpot(BestCoverSpot))
		{
			BlackboardComp->SetValueAsVector(MoveToLocationKey.SelectedKeyName, BestCoverSpot->GetComponentLocation());
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

TArray<AActor*> UPMBTTask_FindCover::FindCoverActors(const AActor* ContextActor, const FVector& SearchCenter, const float& SearchRadius)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> FoundActors;
	
	UKismetSystemLibrary::SphereOverlapActors(ContextActor, SearchCenter, SearchRadius, ObjectTypes,
		nullptr, ActorsToIgnore, FoundActors);

	TArray<AActor*> CoverActors;
	
	for (AActor* Actor : FoundActors)
	{
		if (Actor && Actor->Implements<UPMCoverInterface>())
		{
			CoverActors.Add(Actor);
		}
	}

	return CoverActors;
}

TArray<AActor*> UPMBTTask_FindCover::FilterCoverActors(TArray<AActor*>& CoverActors, const FVector& Target, const float MinDistance, const float MaxDistance)
{
	TArray<AActor*> ValidCover;
	const float MinDistanceSquared = FMath::Square(MinDistance);
	const float MaxDistanceSquared = FMath::Square(MaxDistance);
	
	for (AActor* CoverActor : CoverActors)
	{
		if (ensure(CoverActor))
		{
			const float DistSqr = FVector::DistSquared(CoverActor->GetActorLocation(), Target);
			if (DistSqr >= MinDistanceSquared && DistSqr <= MaxDistanceSquared)
			{
				ValidCover.Add(CoverActor);
			}
		}
	}

	return ValidCover;
}

void UPMBTTask_FindCover::SortCoverActors(TArray<AActor*>& CoverActors, const FVector& TargetLocation,
	const FVector& QuerierLocation, const EPMCoverSearchMode& SearchMode)
{
	switch (SearchMode) {
	case EPMCoverSearchMode::LeastDistanceToQuerier:
		CoverActors.Sort([QuerierLocation](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), QuerierLocation) < FVector::DistSquared(B.GetActorLocation(), QuerierLocation);
		});
		break;
	case EPMCoverSearchMode::GreatestDistanceToQuerier:
		CoverActors.Sort([QuerierLocation](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), QuerierLocation) > FVector::DistSquared(B.GetActorLocation(), QuerierLocation);
		});
		break;
	case EPMCoverSearchMode::LeastDistanceToTarget:
		CoverActors.Sort([TargetLocation](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), TargetLocation) < FVector::DistSquared(B.GetActorLocation(), TargetLocation);
		});
		break;
	case EPMCoverSearchMode::GreatestDistanceToTarget:
		CoverActors.Sort([TargetLocation](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), TargetLocation) > FVector::DistSquared(B.GetActorLocation(), TargetLocation);
		});
		break;
	default:
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMBTTask_FindCover::SortCoverActors -- Not implemented for search mode: %s"),
			*UEnum::GetValueAsString(SearchMode));
	}
}

UPMCoverSpot* UPMBTTask_FindCover::GetBestCoverSpot(const TArray<AActor*>& CoverActors, const FVector& TargetLocation, const AActor* Querier, const bool bTestCoverSpotNavigable)
{
	if (!ensure(Querier)) return nullptr;
	for (const AActor* Cover : CoverActors)
	{
		if (UPMCoverComponent* CoverComponent = IPMCoverInterface::Execute_GetCoverComponent(Cover); ensure(CoverComponent))
		{
			if (UPMCoverSpot* BestCoverSpot = CoverComponent->GetBestCoverSpot(TargetLocation, Querier, bTestCoverSpotNavigable))
			{
				return BestCoverSpot;
			}
		}
		else
		{
			const FString ActorName = Cover ? Cover->GetName() : TEXT("NULL Actor");
			UE_LOG(LogCoverSystem, Error,
				TEXT("UPMBTTask_FindCover::GetBestCoverSpot -- Cover actor does not implement IPMCoverInterface!: %s"),
				*ActorName);
		}
	}
	return nullptr;
}

FVector UPMBTTask_FindCover::GetKeyLocation(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key)
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

float UPMBTTask_FindCover::GetKeyFloatValue(const UBlackboardComponent* BlackboardComp, const FBlackboardKeySelector& Key)
{
	float KeyValue = 0.0f;
	if (!ensure(BlackboardComp)) return KeyValue;

	if (Key.SelectedKeyType == UBlackboardKeyType_Float::StaticClass())
	{
		KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Float>(Key.GetSelectedKeyID());
	}
	
	return KeyValue;
}