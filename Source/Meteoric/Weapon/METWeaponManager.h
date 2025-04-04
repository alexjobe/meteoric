// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "METWeaponManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METEORIC_API UMETWeaponManager : public UActorComponent
{
	GENERATED_BODY()

	/** Next Weapon Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CycleWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> UnarmedCharacterEquipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<class UMETWeaponLoadout> DefaultWeaponLoadout;

public:
	UMETWeaponManager();

	//~ Begin UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	//~ End UActorComponent interface

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class AMETWeapon* const InWeapon, int InSlot);

	void DropAllWeapons();

private:
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipWeapon(AMETWeapon* InWeapon, int InSlot);

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedEvent, AMETWeapon*, NewWeapon);
	FWeaponEquippedEvent& OnWeaponEquippedEvent() { return WeaponEquippedEvent; }
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangingWeaponsEvent, bool, bIsChangingWeapons);
	FChangingWeaponsEvent& OnChangingWeaponsEvent() { return ChangingWeaponsEvent; }

	AMETWeapon* GetCurrentWeapon() const { return CurrentWeapon; }
	bool IsChangingWeapons() const { return bIsChangingWeapons; }
	void OnEquipWeaponNotify();

	void EquipCurrentWeapon();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", ReplicatedUsing=OnRep_CurrentWeapon, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AMETWeapon> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta=(AllowPrivateAccess = "true"))
	int MaxWeapons;
	
	void StartEquipWeapon(AMETWeapon* const InWeapon);
	void FinishEquipWeapon();
	static void UnequipWeapon(AMETWeapon* const InWeapon);

	/** Called for cycle weapon input */
	void CycleWeaponInput(const struct FInputActionValue& Value);
	void CycleWeapon(const bool bInForward);

	UFUNCTION(Server, Reliable)
	void Server_CycleWeapon(const bool bInForward);

	int ChooseEquipSlot() const;

	void GrantLoadout(const UMETWeaponLoadout& InLoadout);

	/**
	 * Makes sure weapons in inventory are attached to player. This is mostly relevant for weapons granted to the player
	 * immediately on spawn.
	*/
	void SetupReserveWeaponAttachments();

	UFUNCTION()
	void InteractionComponent_OnInteractEvent(AActor* InInteractable);

private:
	UPROPERTY(Transient, Replicated)
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient, VisibleAnywhere, ReplicatedUsing=OnRep_Weapons)
	TArray<TObjectPtr<AMETWeapon>> Weapons;

	UPROPERTY(Transient)
	TObjectPtr<AMETWeapon> PreviousWeapon;

	UPROPERTY(Transient, Replicated)
	int CurrentWeaponSlot;

	FWeaponEquippedEvent WeaponEquippedEvent;
	FChangingWeaponsEvent ChangingWeaponsEvent;
	
	bool bIsChangingWeapons;

	UFUNCTION()
	void OnRep_CurrentWeapon(const AMETWeapon* InOldWeapon);

	UFUNCTION()
	void OnRep_Weapons(const TArray<AMETWeapon*>& InOldWeapons);

	void PlayEquipMontage();
	void PlayUnequipMontage();

	void OnEquipMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	void OnUnequipMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);
	static void UnbindMontageCallbacks(const ACharacter* Character, const UAnimMontage* AnimMontage);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
};
