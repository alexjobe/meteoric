// Copyright Alex Jobe


#include "METAbility_FireWeapon.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
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
		return;
	}

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
			const FVector EndLocation = EyesViewpoint.GetLocation() + EyesViewpoint.GetRotation().GetForwardVector() * 2000.f;
			const FRotator StartRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, EndLocation);
			const FTransform SpawnTransform(StartRotation, StartLocation);

			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);
			
			FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(Weapon->DamageEffectClass, GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, METGameplayTags::Damage, Weapon->Damage);
			
			ProjectileWeaponComponent->FireProjectile(SpawnTransform, GetOwningActorFromActorInfo(), MetCharacter, DamageEffectSpecHandle);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
