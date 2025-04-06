// Copyright Alex Jobe


#include "Utils/PMCoverSystemUtils.h"

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

UPMCoverSpot* UPMCoverSystemUtils::FindCover(const FPMFindCoverParams& Params)
{
	UPMCoverUserComponent* CoverUserComponent = Params.ContextActor ? Params.ContextActor->FindComponentByClass<UPMCoverUserComponent>() : nullptr;

	if (!ensure(CoverUserComponent))
	{
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSystemUtils::FindCover -- PMCoverUserComponent not found!"));
		return nullptr;
	}

	// If a cover spot is already reserved and is still valid, renew reservation and return early. This is mostly for
	// situations where the agent briefly steps out of cover to attack, then returns to cover
	if (UPMCoverSpot* ReservedCoverSpot = CoverUserComponent->GetReservedCoverSpot())
	{
		if (ReservedCoverSpot->IsValidCover(Params.TargetLocation) && ReservedCoverSpot->RenewReservation(Params.ContextActor))
		{
			return ReservedCoverSpot;
		}
	}

	// Find all cover actors (IPMCoverInterface) in search radius
	TArray<AActor*> FoundCoverActors = FindCoverActors(Params);
	if (FoundCoverActors.Num() == 0) return nullptr;

	// Filter out cover actors that do not fall within ideal distance to target
	TArray<AActor*> FilteredCoverActors = FilterCoverActors(FoundCoverActors, Params);
	if (FilteredCoverActors.Num() == 0) return nullptr;

	// Sort cover actors by search mode
	SortCoverActors(FilteredCoverActors, Params);

	// Once cover actors are sorted, iterate until we find one with a valid cover spot
	if (UPMCoverSpot* BestCoverSpot = GetBestCoverSpot(FilteredCoverActors, Params))
	{
		if (CoverUserComponent->ReserveCoverSpot(BestCoverSpot))
		{
			return BestCoverSpot;
		}
	}
	return nullptr;
}

TArray<AActor*> UPMCoverSystemUtils::FindCoverActors(const FPMFindCoverParams& Params)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	const TArray<AActor*> ActorsToIgnore;
	TArray<AActor*> FoundActors;
	
	UKismetSystemLibrary::SphereOverlapActors(Params.ContextActor, Params.SearchCenter, Params.SearchRadius,
		ObjectTypes, nullptr, ActorsToIgnore, FoundActors);

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

TArray<AActor*> UPMCoverSystemUtils::FilterCoverActors(TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params)
{
	TArray<AActor*> ValidCover;
	const float MinDistanceSquared = FMath::Square(Params.MinDistanceToTarget);
	const float MaxDistanceSquared = FMath::Square(Params.MaxDistanceToTarget);
	
	for (AActor* CoverActor : CoverActors)
	{
		if (ensure(CoverActor))
		{
			const float DistSqr = FVector::DistSquared(CoverActor->GetActorLocation(), Params.TargetLocation);
			if (DistSqr >= MinDistanceSquared && DistSqr <= MaxDistanceSquared)
			{
				ValidCover.Add(CoverActor);
			}
		}
	}

	return ValidCover;
}

void UPMCoverSystemUtils::SortCoverActors(TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params)
{
	if (!ensure(Params.ContextActor)) return;
	const FVector QuerierLocation = Params.ContextActor->GetActorLocation();
	
	switch (Params.SearchMode) {
	case EPMCoverSearchMode::LeastDistanceToQuerier:
		CoverActors.Sort([&](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), QuerierLocation) < FVector::DistSquared(B.GetActorLocation(), QuerierLocation);
		});
		break;
	case EPMCoverSearchMode::GreatestDistanceToQuerier:
		CoverActors.Sort([&](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), QuerierLocation) > FVector::DistSquared(B.GetActorLocation(), QuerierLocation);
		});
		break;
	case EPMCoverSearchMode::LeastDistanceToTarget:
		CoverActors.Sort([&](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), Params.TargetLocation) < FVector::DistSquared(B.GetActorLocation(), Params.TargetLocation);
		});
		break;
	case EPMCoverSearchMode::GreatestDistanceToTarget:
		CoverActors.Sort([&](const AActor& A, const AActor& B)
		{
			return FVector::DistSquared(A.GetActorLocation(), Params.TargetLocation) > FVector::DistSquared(B.GetActorLocation(), Params.TargetLocation);
		});
		break;
	default:
		UE_LOG(LogCoverSystem, Error,
			TEXT("UPMCoverSystemUtils::SortCoverActors -- Not implemented for search mode: %s"),
			*UEnum::GetValueAsString(Params.SearchMode));
	}
}

UPMCoverSpot* UPMCoverSystemUtils::GetBestCoverSpot(const TArray<AActor*>& CoverActors, const FPMFindCoverParams& Params)
{
	if (!ensure(Params.ContextActor)) return nullptr;
	for (const AActor* Cover : CoverActors)
	{
		if (UPMCoverComponent* CoverComponent = IPMCoverInterface::Execute_GetCoverComponent(Cover); ensure(CoverComponent))
		{
			const bool bCoverAvailable = CoverComponent->IsCoverAvailable(Params.ContextActor);
			if (UPMCoverSpot* BestCoverSpot = bCoverAvailable ? CoverComponent->GetBestCoverSpot(Params.TargetLocation, Params.ContextActor, Params.bTestCoverSpotNavigable) : nullptr)
			{
				return BestCoverSpot;
			}
		}
		else
		{
			const FString ActorName = Cover ? Cover->GetName() : TEXT("NULL Actor");
			UE_LOG(LogCoverSystem, Error,
				TEXT("UPMCoverSystemUtils::GetBestCoverSpot -- Cover actor does not implement IPMCoverInterface!: %s"),
				*ActorName);
		}
	}
	return nullptr;
}
