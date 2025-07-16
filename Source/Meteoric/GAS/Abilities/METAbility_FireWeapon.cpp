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

UMETAbility_FireWeapon::UMETAbility_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputTag = METGameplayTags::InputTag_FireWeapon;
	ActivationOwnedTags.AddTag(METGameplayTags::Ability_FireWeapon);
	const FGameplayTagContainer MyAbilityTags(METGameplayTags::Ability_FireWeapon);
	SetAssetTags(MyAbilityTags);
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

	/* Automatic weapon */
	if (ActivationPolicy == EMETAbilityActivationPolicy::OnInputTriggered)
	{
		BindWaitFireCooldownEventTask();
		BindWaitInputReleaseTask();
	}

	BindWaitFireAnimNotifyTask();

	StartFireWeapon();
}

void UMETAbility_FireWeapon::StartFireWeapon()
{
	AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo();
	if (!ensure(MetCharacter) || !MetCharacter->CanFireWeapon())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	const bool bHeld = ActivationPolicy == EMETAbilityActivationPolicy::OnInputTriggered;
	MetCharacter->StartFireWeapon(bHeld);
}

void UMETAbility_FireWeapon::SpawnProjectiles() const
{
	const AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo();
	const AMETWeapon* Weapon = MetCharacter ? MetCharacter->GetWeapon() : nullptr;
	const UMETProjectileWeaponComponent* ProjectileWeaponComponent = Weapon ? Weapon->GetProjectileWeaponComponent() : nullptr;
	
	if (ensure(ProjectileWeaponComponent))
	{
		TArray<FMETSpawnProjectileParams> ProjectileParams;
		for (int Projectile = 0; Projectile < ProjectileWeaponComponent->GetNumProjectiles(); ++Projectile)
		{
			// Create params for each projectile (Some weapons fire multiple projectiles per shot, ex. shotgun)
			ProjectileParams.Push(CreateProjectileSpawnParams());
		}
		
		ProjectileWeaponComponent->FireProjectiles(ProjectileParams);
	}
}

FMETSpawnProjectileParams UMETAbility_FireWeapon::CreateProjectileSpawnParams() const
{
	FMETSpawnProjectileParams SpawnParams;
	
	AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo();
	const AMETWeapon* Weapon = MetCharacter ? MetCharacter->GetWeapon() : nullptr;
	const UMETWeaponAmmoComponent* AmmoComponent = Weapon ? Weapon->GetAmmoComponent() : nullptr;

	const FMETAmmoDamageConfig* ImpactDamageConfig = AmmoComponent ? AmmoComponent->GetImpactDamageConfig() : nullptr;
	const FMETAmmoDamageConfig* DelayedDamageConfig = AmmoComponent ? AmmoComponent->GetDelayedDamageConfig() : nullptr;
	
	const TSubclassOf<UGameplayEffect> ImpactDamageEffectClass = ImpactDamageConfig ? ImpactDamageConfig->DamageEffectClass : nullptr;
	const TSubclassOf<UGameplayEffect> DelayedDamageEffectClass = DelayedDamageConfig ? DelayedDamageConfig->DamageEffectClass : nullptr;

	SpawnParams.Owner = GetOwningActorFromActorInfo();
	SpawnParams.Instigator = MetCharacter;

	if (ImpactDamageEffectClass)
	{
		SpawnParams.ImpactDamageHandle.EffectHandle = MakeDamageEffectSpecHandle(ImpactDamageEffectClass, ImpactDamageConfig->Damage);
		SpawnParams.ImpactDamageHandle.bExplosive = ImpactDamageConfig->bExplosive;
		SpawnParams.ImpactDamageHandle.ExplosionRadius = ImpactDamageConfig->ExplosionRadius;
	}

	if (DelayedDamageEffectClass)
	{
		SpawnParams.DelayedDamageHandle.EffectHandle = MakeDamageEffectSpecHandle(DelayedDamageEffectClass, DelayedDamageConfig->Damage);
		SpawnParams.DelayedDamageHandle.bExplosive = DelayedDamageConfig->bExplosive;
		SpawnParams.DelayedDamageHandle.ExplosionRadius = DelayedDamageConfig->ExplosionRadius;
	}

	return SpawnParams;
}

bool UMETAbility_FireWeapon::BindWaitFireAnimNotifyTask()
{
	if (const auto WaitGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, METGameplayTags::EventTag_FireAnimNotify))
	{
		WaitGameplayEventTask->EventReceived.AddUniqueDynamic(this, &ThisClass::OnFireAnimNotify);
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

void UMETAbility_FireWeapon::OnFireAnimNotify(FGameplayEventData Payload)
{
	const bool bHasAuthority = GetAvatarActorFromActorInfo()->HasAuthority();
	if (bHasAuthority)
	{
		SpawnProjectiles();
	}

	const bool bHeld = ActivationPolicy == EMETAbilityActivationPolicy::OnInputTriggered;
	if (const AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo(); ensure(MetCharacter))
	{
		MetCharacter->FinishFireWeapon(bHeld);
	}

	/* Single shot weapon */
	if (ActivationPolicy == EMETAbilityActivationPolicy::OnInputStarted && bHasAuthority)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UMETAbility_FireWeapon::OnFireCooldown(FGameplayEventData Payload)
{
	StartFireWeapon();
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
