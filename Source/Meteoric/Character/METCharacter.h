// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "METCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMETCharacter, Log, All);

UCLASS(config=Game)
class METEORIC_API AMETCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AMETCharacter();

	//~ Begin IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface interface

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	//~ End ACharacter interface
	
	class UMETWeaponManager* GetWeaponManager() const { return WeaponManager; }
	class UMETAmmoManager* GetAmmoManager() const { return AmmoManager; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	class AMETWeapon* GetWeapon() const;

	/* Aim down sights */
	UFUNCTION(BlueprintCallable)
	void SetAiming(bool bInIsAiming);
	
	bool IsAiming() const { return bIsAiming; }
	bool IsTurningInPlace() const { return bIsTurningInPlace; }
	bool IsMoving() const;
	bool IsDead() const;
	bool ShouldEnableLeftHandIK() const;

	UFUNCTION(BlueprintPure)
	bool CanFireWeapon() const;

	UFUNCTION(BlueprintCallable)
	void FireWeapon(bool bInHeld);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimDownSightsEvent, bool, bIsAiming);
	FAimDownSightsEvent& OnAimDownSights() { return AimDownSightsEvent; }

	FRotator GetActorControlRotationDelta() const { return ActorControlRotationDelta; }
	virtual FVector GetFocalPoint() const;
	virtual FTransform GetEyesPosition() const;

	//~ Begin UGenericTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	//~ End UGenericTeamAgentInterface interface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UMETAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UMETAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMETWeaponManager> WeaponManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ammo", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMETAmmoManager> AmmoManager;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsAiming, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	bool bIsAiming;

	/* Abilities granted by this character. Added on possession, and removed on death */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> CharacterAbilities;

	/* Default attributes applied on initialization */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	/* Default max attributes - applied before other attributes for clamping */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DefaultMaxAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Death", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;

	virtual void AddCharacterAbilities();
	void RemoveCharacterAbilities() const;

	virtual void InitAbilityActorInfo();
	void InitializeDefaultAttributes() const;

	/** Called for movement input */
	void Move(const struct FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Die();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(bool bInHeld);

	UFUNCTION()
	void OnRep_IsAiming();

	UFUNCTION()
	void WeaponManager_OnChangingWeaponsEvent(bool bInIsChangingWeapons);

	void BindAttributeChangedCallbacks();

private:
	UPROPERTY(Replicated)
	FRotator RepControlRotation;
	
	FRotator ActorControlRotationDelta;
	
	FAimDownSightsEvent AimDownSightsEvent;

	UPROPERTY(Replicated)
	bool bIsTurningInPlace;

	void UpdateCharacterAimRotation(float DeltaSeconds);

	void UpdateActorControlRotationDelta();
	bool IsActorControlRotationAligned() const;

	// Attribute changed callbacks
	void HealthChanged(const struct FOnAttributeChangeData& Data);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
