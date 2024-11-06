// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "EnhancedInputComponent.h"
#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UMETWeaponManager::UMETWeaponManager()
	: MaxWeapons(2)
	, CurrentWeaponSlot(0)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	Weapons.Init(nullptr, MaxWeapons);
}

void UMETWeaponManager::InitializeComponent()
{
	OwningCharacter = Cast<ACharacter>(GetOwner());
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;

	UnequipCurrentWeapon();

	CurrentWeapon = InWeapon;
	CurrentWeapon->OnEquipped(OwningCharacter);
	CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	WeaponEquippedEvent.Broadcast(CurrentWeapon);

	CurrentWeapon->GetMesh()->SetVisibility(true, true);

	Weapons[InSlot] = CurrentWeapon;
	CurrentWeaponSlot = InSlot;
}

void UMETWeaponManager::UnequipCurrentWeapon()
{
	if(!CurrentWeapon) return;
	CurrentWeapon->OnUnequipped();
	CurrentWeapon->GetMesh()->SetVisibility(false, true);
	CurrentWeapon = nullptr;
}

void UMETWeaponManager::CycleWeapon(bool bInNext)
{
	if(MaxWeapons <= 1) return;
	
	int NewSlot = CurrentWeaponSlot + (bInNext ? 1 : -1);
	if(NewSlot < 0) NewSlot = MaxWeapons - 1;
	if(NewSlot >= MaxWeapons) NewSlot = 0;

	CurrentWeaponSlot = NewSlot;
	
	if(Weapons[NewSlot] != nullptr)
	{
		EquipWeapon(Weapons[NewSlot], NewSlot);
	}
	
	//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("CurrentWeaponSlot: %i"), CurrentWeaponSlot));
}

void UMETWeaponManager::CycleWeaponInput(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	CycleWeapon(Axis > 0);
}

void UMETWeaponManager::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Started, this, &UMETWeaponManager::CycleWeaponInput);
	}
}
