// Copyright Alex Jobe


#include "METAbility_FireWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/Character/METCharacter.h"
#include "Meteoric/Weapon/Projectile/METProjectileWeaponComponent.h"
#include "Meteoric/Weapon/METWeapon.h"
#include "Meteoric/Weapon/Ammo/METWeaponAmmoComponent.h"
#include "Meteoric/Weapon/Handling/METWeaponSpreadComponent.h"

UMETAbility_FireWeapon::UMETAbility_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputTag = METGameplayTags::InputTag_FireWeapon;
	ActivationOwnedTags.AddTag(METGameplayTags::Ability_FireWeapon);
	AbilityTags.AddTag(METGameplayTags::Ability_FireWeapon);
	CancelAbilitiesWithTag.AddTag(METGameplayTags::Ability_ReloadWeapon);
}

void UMETAbility_FireWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (ActivationPolicy == SingleShot)
	{
		FireWeapon();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	/* Automatic weapon */
	if (BindWaitFireCooldownEventTask())
	{
		FireWeapon();
		BindWaitInputReleaseTask();
	}
}

void UMETAbility_FireWeapon::FireWeapon()
{
	AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo();
	if (!ensure(MetCharacter) || !MetCharacter->CanFireWeapon())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	MetCharacter->FireWeapon(ActivationPolicy == OnInputHeld);

	if (GetAvatarActorFromActorInfo()->HasAuthority())
	{
		const AMETWeapon* Weapon = MetCharacter->GetWeapon();
		const UMETProjectileWeaponComponent* ProjectileWeaponComponent = Weapon ? Weapon->GetProjectileWeaponComponent() : nullptr;
		UMETWeaponSpreadComponent* WeaponSpreadComponent = Weapon ? Weapon->GetWeaponSpreadComponent() : nullptr;
		const UMETWeaponAmmoComponent* AmmoComponent = Weapon ? Weapon->GetAmmoComponent() : nullptr;
		const TSubclassOf<UGameplayEffect> ImpactDamageEffectClass = AmmoComponent ? AmmoComponent->GetImpactDamageEffectClass() : nullptr;
		const TSubclassOf<UGameplayEffect> DelayedDamageEffectClass = AmmoComponent ? AmmoComponent->GetDelayedDamageEffectClass() : nullptr;

		if (ensure(AmmoComponent) && ensure(ProjectileWeaponComponent) && ensure(WeaponSpreadComponent))
		{
			const FGameplayEffectSpecHandle ImpactDamageEffectSpecHandle = MakeDamageEffectSpecHandle(ImpactDamageEffectClass, AmmoComponent->GetImpactDamage());
			const FGameplayEffectSpecHandle DelayedDamageEffectSpecHandle = MakeDamageEffectSpecHandle(DelayedDamageEffectClass, AmmoComponent->GetDelayedDamage());

			FMETSpawnProjectileParams SpawnParams;
			SpawnParams.SpawnTransform = WeaponSpreadComponent->GetProjectileSpawnTransform();
			SpawnParams.Owner = GetOwningActorFromActorInfo();
			SpawnParams.Instigator = MetCharacter;
			SpawnParams.ImpactDamageEffectHandle = ImpactDamageEffectSpecHandle;
			SpawnParams.DelayedDamageEffectHandle = DelayedDamageEffectSpecHandle;
			
			ProjectileWeaponComponent->FireProjectile(SpawnParams);
			WeaponSpreadComponent->OnWeaponFired();
		}
	}
}

bool UMETAbility_FireWeapon::BindWaitFireCooldownEventTask()
{
	if (const auto WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, METGameplayTags::EventTag_FireCooldown))
	{
		WaitGameplayEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnFireCooldown);
		WaitGameplayEventTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogMETAbilitySystem, Error, TEXT("UMETAbility_FireWeapon: Failed to bind WaitGameplayEventTask"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return false;
	}
	return true;
}

bool UMETAbility_FireWeapon::BindWaitInputReleaseTask()
{
	if (const auto WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true))
	{
		WaitInputReleaseTask->OnRelease.AddUniqueDynamic(this, &ThisClass::OnInputReleased);
		WaitInputReleaseTask->ReadyForActivation();
	}
	else
	{
		UE_LOG(LogMETAbilitySystem, Error, TEXT("UMETAbility_FireWeapon: Failed to bind WaitInputReleaseTask"))
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return false;
	}
	return true;
}

void UMETAbility_FireWeapon::OnFireCooldown(FGameplayEventData Payload)
{
	FireWeapon();
}

void UMETAbility_FireWeapon::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

FGameplayEffectSpecHandle UMETAbility_FireWeapon::MakeDamageEffectSpecHandle(const TSubclassOf<UGameplayEffect>& InDamageEffectClass, const float InMagnitude) const
{
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	if (InDamageEffectClass)
	{
		DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(InDamageEffectClass, GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, METGameplayTags::Damage, InMagnitude);
	}

	return DamageEffectSpecHandle;
}
