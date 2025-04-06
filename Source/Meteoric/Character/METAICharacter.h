// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "METCharacter.h"
#include "Interface/PuppetMasterInterface.h"
#include "METAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class METEORIC_API AMETAICharacter : public AMETCharacter, public IPuppetMasterInterface
{
	GENERATED_BODY()

public:
	AMETAICharacter();

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	//~ End ACharacter interface
	
	//~ Begin AMETCharacter interface
	virtual FVector GetFocalPoint() const override;
	//~ End AMETCharacter interface

	//~ Begin IPuppetMasterInterface interface
	UFUNCTION(BlueprintCallable)
	virtual UPMPuppetComponent* GetPuppetComponent() const override;
	//~ End IPuppetMasterInterface interface

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Death")
	float CorpseLifeSpan;

	UPROPERTY(Transient)
	TObjectPtr<class AMETAIController> AIController;
	
	//~ Begin AMETCharacter interface
	virtual void InitAbilityActorInfo() override;
	virtual void Die() override;
	virtual void AttributeSet_OnAttributeDamageEvent(const float DamageAmount, const FGameplayEffectSpec& EffectSpec) override;
	//~ End AMETCharacter interface

private:
	UFUNCTION()
	void AmmoManager_OnWeaponAmmoChanged(class UMETAmmoDataAsset* const AmmoType, int32 AmmoCount, int32 MaxAmmo);
};
