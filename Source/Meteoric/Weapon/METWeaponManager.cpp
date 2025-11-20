// Copyright Alex Jobe


#include "METWeaponManager.h"

#include "EnhancedInputComponent.h"
#include "METWeapon.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Loadout/METWeaponLoadout.h"
#include "Meteoric/METLogChannels.h"
#include "Meteoric/Interaction/METInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

UMETWeaponManager::UMETWeaponManager()
	: MaxWeapons(2)
	, CurrentWeaponSlot(0)
	, bIsChangingWeapons(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void UMETWeaponManager::InitializeComponent()
{
	Super::InitializeComponent();
	OwningCharacter = Cast<ACharacter>(GetOwner());
	if(UMETInteractionComponent* InteractionComponent = OwningCharacter->FindComponentByClass<UMETInteractionComponent>())
	{
		InteractionComponent->OnInteractEvent().AddUniqueDynamic(this, &UMETWeaponManager::InteractionComponent_OnInteractEvent);
	}
	Weapons.Init(nullptr, MaxWeapons);
}

void UMETWeaponManager::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultWeaponLoadout)
	{
		GrantLoadout(*DefaultWeaponLoadout);
	}
}

void UMETWeaponManager::EquipWeapon(AMETWeapon* const InWeapon, int InSlot)
{
	if(!ensure(InSlot >= 0 && InSlot < MaxWeapons)) return;
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	if(bIsChangingWeapons) return;

	bIsChangingWeapons = true;
	ChangingWeaponsEvent.Broadcast(bIsChangingWeapons);

	if(Weapons[InSlot] == CurrentWeapon && CurrentWeapon != nullptr && InWeapon != CurrentWeapon)
	{
		CurrentWeapon->Drop();
		Weapons[InSlot] = nullptr;
		CurrentWeapon = nullptr;
		PreviousWeapon = nullptr;
	}

	if (InWeapon != CurrentWeapon)
	{
		PreviousWeapon = CurrentWeapon;
	}
	
	if (PreviousWeapon)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PreviousWeapon->FXSettings.UnequipSound, PreviousWeapon->GetActorLocation());
	}
	
	StartEquipWeapon(InWeapon);

	Weapons[InSlot] = CurrentWeapon;
	CurrentWeaponSlot = InSlot;

	if(GetOwner()->HasAuthority())
	{
		Multicast_EquipWeapon(InWeapon, InSlot);
	}
}

void UMETWeaponManager::DropAllWeapons()
{
	CurrentWeapon = nullptr;
	PreviousWeapon = nullptr;
	for (int Index = 0; Index < Weapons.Num(); Index++)
	{
		if(Weapons[Index] != nullptr)
		{
			Weapons[Index]->Drop();
			Weapons[Index] = nullptr;
		}
	}

	WeaponEquippedEvent.Broadcast(nullptr);
}

void UMETWeaponManager::Multicast_EquipWeapon_Implementation(AMETWeapon* InWeapon, int InSlot)
{
	if(!GetOwner()->HasAuthority())
	{
		EquipWeapon(InWeapon, InSlot);
	}
}

void UMETWeaponManager::StartEquipWeapon(AMETWeapon* const InWeapon)
{
	if(!ensure(OwningCharacter) || !ensure(InWeapon)) return;
	CurrentWeapon = InWeapon;
	
	CurrentWeapon->GetMesh()->SetVisibility(false, true);

	if (PreviousWeapon || UnarmedCharacterEquipMontage)
	{
		PlayUnequipMontage();
	}
	else
	{
		PlayEquipMontage();
	}
}

void UMETWeaponManager::OnRep_CurrentWeapon(const AMETWeapon* InOldWeapon)
{
	EquipCurrentWeapon();
}

void UMETWeaponManager::OnRep_Weapons(const TArray<AMETWeapon*>& InOldWeapons)
{
	if (Weapons != InOldWeapons)
	{
		// Attaches reserve weapons to player if they are not already attached.
		SetupReserveWeaponAttachments();
	}
}

void UMETWeaponManager::PlayEquipMontage()
{
	UAnimMontage* EquipMontage = CurrentWeapon ? CurrentWeapon->GetCharacterEquipWeaponMontage() : UnarmedCharacterEquipMontage.Get();
	if (!ensure(EquipMontage) || !ensure(OwningCharacter)) return;
	
	OwningCharacter->PlayAnimMontage(EquipMontage, 1, FName("Equip"));
	
	const USkeletalMeshComponent* SkeletalMeshComponent = OwningCharacter->GetMesh();
	const UAnimInstance* AnimInstance = SkeletalMeshComponent ? SkeletalMeshComponent->GetAnimInstance() : nullptr;
	
	if(FAnimMontageInstance* MontageInstance = AnimInstance ? AnimInstance->GetInstanceForMontage(EquipMontage) : nullptr; ensure(MontageInstance))
	{
		MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &UMETWeaponManager::OnEquipMontageEnded);
		MontageInstance->OnMontageEnded.BindUObject(this, &UMETWeaponManager::OnEquipMontageEnded);
	}
}

void UMETWeaponManager::PlayUnequipMontage()
{
	UAnimMontage* UnequipMontage = PreviousWeapon ? PreviousWeapon->GetCharacterEquipWeaponMontage() : UnarmedCharacterEquipMontage.Get();
	if (!ensure(UnequipMontage) || !ensure(OwningCharacter)) return;
	
	OwningCharacter->PlayAnimMontage(UnequipMontage, 1, FName("Unequip"));

	const USkeletalMeshComponent* SkeletalMeshComponent = OwningCharacter->GetMesh();
	const UAnimInstance* AnimInstance = SkeletalMeshComponent ? SkeletalMeshComponent->GetAnimInstance() : nullptr;
	
	if(FAnimMontageInstance* MontageInstance = AnimInstance ? AnimInstance->GetInstanceForMontage(UnequipMontage) : nullptr; ensure(MontageInstance))
	{
		MontageInstance->OnMontageBlendingOutStarted.BindUObject(this, &UMETWeaponManager::OnUnequipMontageEnded);
		MontageInstance->OnMontageEnded.BindUObject(this, &UMETWeaponManager::OnUnequipMontageEnded);
	}
}

void UMETWeaponManager::OnEquipMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	UnbindMontageCallbacks(OwningCharacter, AnimMontage);
	FinishEquipWeapon();
}

void UMETWeaponManager::OnUnequipMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	UnbindMontageCallbacks(OwningCharacter, AnimMontage);
	PlayEquipMontage();
}

void UMETWeaponManager::UnbindMontageCallbacks(const ACharacter* Character, const UAnimMontage* AnimMontage)
{
	const USkeletalMeshComponent* SkeletalMeshComponent = Character ? Character->GetMesh() : nullptr;
	const UAnimInstance* AnimInstance = SkeletalMeshComponent ? SkeletalMeshComponent->GetAnimInstance() : nullptr;
	if(FAnimMontageInstance* MontageInstance = AnimInstance ? AnimInstance->GetInstanceForMontage(AnimMontage) : nullptr)
	{
		MontageInstance->OnMontageBlendingOutStarted.Unbind();
		MontageInstance->OnMontageEnded.Unbind();
	}
}

void UMETWeaponManager::OnEquipWeaponNotify()
{
	if(!ensure(OwningCharacter) || !ensure(CurrentWeapon)) return;

	UnequipWeapon(PreviousWeapon);
	PreviousWeapon = nullptr;
	CurrentWeapon->SetWeaponDroppedState(false);
	CurrentWeapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, CurrentWeapon->ParentAttachmentSocket);
	CurrentWeapon->GetMesh()->SetVisibility(true, true);
	CurrentWeapon->OnEquipped(OwningCharacter);
	UGameplayStatics::PlaySoundAtLocation(this, CurrentWeapon->FXSettings.EquipSound, CurrentWeapon->GetActorLocation());
	WeaponEquippedEvent.Broadcast(CurrentWeapon);
}

void UMETWeaponManager::EquipCurrentWeapon()
{
	if (CurrentWeapon && !CurrentWeapon->IsEquipped() && Weapons[CurrentWeaponSlot] == CurrentWeapon)
	{
		EquipWeapon(CurrentWeapon, CurrentWeaponSlot);
	}
}

void UMETWeaponManager::FinishEquipWeapon()
{
	if(!bIsChangingWeapons) return;
	bIsChangingWeapons = false;
	ChangingWeaponsEvent.Broadcast(bIsChangingWeapons);
}

void UMETWeaponManager::UnequipWeapon(AMETWeapon* const InWeapon)
{
	if(!InWeapon) return;
	InWeapon->OnUnequipped();
	InWeapon->GetMesh()->SetVisibility(false, true);
}

void UMETWeaponManager::CycleWeaponInput(const FInputActionValue& Value)
{
	const bool bNext = Value.Get<float>() > 0.f;;

	if(GetOwner()->HasAuthority())
	{
		CycleWeapon(bNext);
	}
	else
	{
		Server_CycleWeapon(bNext);
	}
}

void UMETWeaponManager::CycleWeapon(const bool bInForward)
{
	if(MaxWeapons <= 1) return;
	if(bIsChangingWeapons) return;
	
	int NewSlot = CurrentWeaponSlot + (bInForward ? 1 : -1);
	if(NewSlot < 0) NewSlot = MaxWeapons - 1;
	if(NewSlot >= MaxWeapons) NewSlot = 0;

	CurrentWeaponSlot = NewSlot;
	
	if(Weapons[NewSlot] != nullptr)
	{
		EquipWeapon(Weapons[NewSlot], NewSlot);
	}
}

void UMETWeaponManager::Server_CycleWeapon_Implementation(const bool bInForward)
{
	CycleWeapon(bInForward);
}

int UMETWeaponManager::ChooseEquipSlot() const
{
	if(Weapons[CurrentWeaponSlot] != nullptr)
	{
		for(int Slot = 0; Slot < MaxWeapons; ++Slot)
		{
			if(Weapons[Slot] == nullptr) return Slot;
		}
	}

	return CurrentWeaponSlot;
}

void UMETWeaponManager::GrantLoadout(const UMETWeaponLoadout& InLoadout)
{
	if(!GetOwner()->HasAuthority()) return;
	
	if (InLoadout.Weapons.Num() > MaxWeapons)
	{
		UE_LOG(LogMET, Warning, TEXT("METWeaponManager: Could not grant full weapon loadout - loadout size is greater than MaxWeapons"));
	}

	for (int Index = 0; Index < FMath::Min(InLoadout.Weapons.Num(), MaxWeapons); ++Index)
	{
		const auto& WeaponClass = InLoadout.Weapons[Index];
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (AMETWeapon* NewWeapon = GetWorld()->SpawnActor<AMETWeapon>(WeaponClass, SpawnParameters); ensure(NewWeapon))
		{
			Weapons[Index] = NewWeapon;
		}
	}

	if (GetOwner()->HasAuthority())
	{
		if (!Weapons.IsEmpty() && Weapons[0] != nullptr)
		{
			EquipWeapon(Weapons[0], 0);
		}
	}
	
	SetupReserveWeaponAttachments();
}

void UMETWeaponManager::SetupReserveWeaponAttachments()
{
	if (!ensure(OwningCharacter)) return;
	
	for (int Index = 0; Index < Weapons.Num(); ++Index)
	{
		AMETWeapon* Weapon = Weapons[Index];
		if(Weapon != nullptr && Weapon != CurrentWeapon && !Weapon->IsAttachedTo(OwningCharacter))
		{
			Weapon->GetMesh()->SetVisibility(false, true);
			Weapon->SetActorTickEnabled(false);
			Weapon->SetWeaponDroppedState(false);
			Weapon->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Weapon->ParentAttachmentSocket);
		}
	}
}

void UMETWeaponManager::InteractionComponent_OnInteractEvent(AActor* InInteractable)
{
	if(bIsChangingWeapons) return;

	if (GetOwner()->HasAuthority())
	{
		AMETWeapon* NewWeapon = Cast<AMETWeapon>(InInteractable);
		if(!NewWeapon) return;

		const int EquipSlot = ChooseEquipSlot();
		EquipWeapon(NewWeapon, EquipSlot);
	}
}

void UMETWeaponManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMETWeaponManager, CurrentWeapon)
	DOREPLIFETIME(UMETWeaponManager, OwningCharacter)
	DOREPLIFETIME(UMETWeaponManager, Weapons)
	DOREPLIFETIME(UMETWeaponManager, CurrentWeaponSlot)
}

void UMETWeaponManager::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &UMETWeaponManager::CycleWeaponInput);
	}
}