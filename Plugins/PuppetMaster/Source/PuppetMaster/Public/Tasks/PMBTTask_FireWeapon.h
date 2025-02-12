// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PMBTTask_FireWeapon.generated.h"

struct FBTFireWeaponMemory
{
	float TimeToFireFor;
	float TimeStartedFire;

	FBTFireWeaponMemory()
		: TimeToFireFor(0.f)
		, TimeStartedFire(0.f)
	{}
};

UCLASS(MinimalAPI)
class UPMBTTask_FireWeapon : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	PUPPETMASTER_API UPMBTTask_FireWeapon(const FObjectInitializer& ObjectInitializer);

	//~ Begin UBTTaskNode interface
	PUPPETMASTER_API virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	PUPPETMASTER_API virtual uint16 GetInstanceMemorySize() const override;
	PUPPETMASTER_API virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	PUPPETMASTER_API virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	//~ End UBTTaskNode interface

	UPROPERTY(EditAnywhere, Category = "Node")
	FGameplayTag FireAbilityTag;
	
	UPROPERTY(EditAnywhere, Category = "Node")
	float TimeToFireFor;
};
