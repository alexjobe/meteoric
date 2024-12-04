// Copyright Alex Jobe


#include "METAbility_FireWeapon.h"

#include "Meteoric/METGameplayTags.h"
#include "Meteoric/Character/METCharacter.h"
#include "Meteoric/Weapon/METProjectileWeaponComponent.h"
#include "Meteoric/Weapon/METWeapon.h"

UMETAbility_FireWeapon::UMETAbility_FireWeapon()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	InputTag = METGameplayTags::InputTag_FireWeapon;
}

void UMETAbility_FireWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	AMETCharacter* MetCharacter = GetMetCharacterFromActorInfo();
	if (!ensure(MetCharacter) || !MetCharacter->CanFireWeapon())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	MetCharacter->FireWeapon(ActivationPolicy == OnInputHeld);

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{
		const AMETWeapon* Weapon = MetCharacter->GetWeapon();
		const UMETProjectileWeaponComponent* ProjectileWeaponComponent = Weapon->GetProjectileWeaponComponent();

		if (ensure(Weapon) && ensure(Weapon->DamageEffectClass) && ensure(ProjectileWeaponComponent))
		{
			FTransform MuzzleLocation = Weapon->GetMesh()->GetSocketTransform(FName("S_Muzzle"));
			FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(Weapon->DamageEffectClass, GetAbilityLevel());
			ProjectileWeaponComponent->FireProjectile(MuzzleLocation, GetOwningActorFromActorInfo(), MetCharacter, DamageEffectSpecHandle);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
