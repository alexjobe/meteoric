// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/Character.h"
#include "Meteoric/Weapon/METWeaponManager.h"
#include "METCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMETCharacter, Log, All);

UCLASS(config=Game)
class METEORIC_API AMETCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMETCharacter();

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	virtual void Tick(float DeltaSeconds) override;
	
	UMETWeaponManager* GetWeaponManager() const { return WeaponManager; }
	AMETWeapon* GetWeapon() const { return WeaponManager ? WeaponManager->GetCurrentWeapon() : nullptr;}

	/* Aim down sights */
	UFUNCTION(BlueprintCallable)
	void SetAiming(bool bInIsAiming);
	
	bool IsAiming() const { return bIsAiming; }
	bool IsTurningInPlace() const { return bIsTurningInPlace; }

	UFUNCTION(BlueprintPure)
	bool CanFireWeapon() const;

	UFUNCTION(BlueprintCallable)
	void FireWeapon(bool bInHeld);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimDownSightsEvent, bool, bIsAiming);
	FAimDownSightsEvent& OnAimDownSights() { return AimDownSightsEvent; }

	FRotator GetActorControlRotationDelta() const { return ActorControlRotationDelta; }

protected:
	UPROPERTY(Transient)
	TObjectPtr<class UMETAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponManager> WeaponManager;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsAiming, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> CharacterAbilities;

	/* Default attributes applied on initialization */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DefaultAttributes;

	/* Default max attributes - applied before other attributes for clamping */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DefaultMaxAttributes;

	virtual void AddCharacterAbilities();
	
	void InitializeDefaultAttributes() const;

	/** Called for movement input */
	void Move(const struct FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(bool bInHeld);

	UFUNCTION()
	void OnRep_IsAiming();

	UFUNCTION()
	void WeaponManager_OnChangingWeaponsEvent(bool bInIsChangingWeapons);

private:
	UPROPERTY(Replicated)
	FRotator RepControlRotation;
	
	FRotator ActorControlRotationDelta;
	
	FAimDownSightsEvent AimDownSightsEvent;

	UPROPERTY(Replicated)
	bool bIsTurningInPlace;

	void UpdateActorControlRotationDelta();
	bool IsActorControlRotationAligned() const;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
