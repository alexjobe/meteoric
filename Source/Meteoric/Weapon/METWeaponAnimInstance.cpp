// Copyright Alex Jobe


#include "METWeaponAnimInstance.h"

#include "METWeapon.h"
#include "GameFramework/Character.h"

UMETWeaponAnimInstance::UMETWeaponAnimInstance()
	: bReloading(false)
{
	
}

void UMETWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AMETWeapon>(GetOwningActor());
	if (!Weapon) return;

	Weapon->OnReload().AddUniqueDynamic(this, &ThisClass::Weapon_OnReloadEvent);
}

void UMETWeaponAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bReloading)
	{
		const ACharacter* OwningCharacter = Weapon ? Weapon->GetOwningCharacter() : nullptr;
		const USkeletalMeshComponent* CharacterMesh = OwningCharacter ? OwningCharacter->GetMesh() : nullptr;
		if (ensure(CharacterMesh))
		{
			AmmoWorldTransform = CharacterMesh->GetSocketTransform(FName("S_Ammo"));
			//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("AmmoWorldTransform: %s"), *AmmoWorldTransform.ToString()));
		}
	}
}

void UMETWeaponAnimInstance::Weapon_OnReloadEvent(const bool bInReloading)
{
	bReloading = bInReloading;
}
