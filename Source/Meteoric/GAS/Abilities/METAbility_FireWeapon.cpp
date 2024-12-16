// Copyright Alex Jobe


#include "METAbility_FireWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Kismet/KismetMathLibrary.h"
#include "Meteoric/METGameplayTags.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/Character/METCharacter.h"
#include "Meteoric/Weapon/METProjectileWeaponComponent.h"
#include "Meteoric/Weapon/METWeapon.h"

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
		const UMETProjectileWeaponComponent* ProjectileWeaponComponent = Weapon->GetProjectileWeaponComponent();

		if (ensure(Weapon) && ensure(Weapon->DamageEffectClass) && ensure(ProjectileWeaponComponent))
		{
			const FVector StartLocation = Weapon->GetMesh()->GetSocketLocation(FName("S_Muzzle"));
			const FTransform EyesViewpoint = MetCharacter->GetEyesViewpoint();
			constexpr float Range = 3000.f;
			const FVector EndLocation = EyesViewpoint.GetLocation() + EyesViewpoint.GetRotation().GetForwardVector() * Range;
			const FRotator StartRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);
			const FTransform SpawnTransform(StartRotation, StartLocation);

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);
			
			FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(Weapon->DamageEffectClass, GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, METGameplayTags::Damage, Weapon->Damage);
			
			ProjectileWeaponComponent->FireProjectile(SpawnTransform, GetOwningActorFromActorInfo(), MetCharacter, DamageEffectSpecHandle);
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
