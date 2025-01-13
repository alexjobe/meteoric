// Copyright Alex Jobe


#include "METRecoilComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"
#include "Meteoric/METGameplayTags.h"

UMETRecoilComponent::UMETRecoilComponent()
	: AimRecoilCurve(nullptr)
	, VerticalAimRecoilMultiplier(1.f)
	, HorizontalAimRecoilMultiplier(1.f)
	, AimRecoilNoise(0.01f)
	, RecoilForce_Z(100.f)
	, RecoilForce_Y(200.f)
	, RecoilForce_Pitch(100.f)
	, FiringMode(SingleShot)
	, FireActionStartTime(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	RecoilSpring_Z.Initialize();
	RecoilSpring_Y.Initialize();
	RecoilSpring_Pitch.Initialize();
}

void UMETRecoilComponent::UpdateRecoil(float InDeltaTime)
{
	constexpr float TranslationEaseExp = 10.f;
	constexpr float RotationEaseExp = 20.f;

	FVector RecoilTranslation = SpringRecoilTransform.GetTranslation();
	RecoilTranslation.Y -= RecoilSpring_Y.Update(InDeltaTime);
	RecoilTranslation.Z += RecoilSpring_Z.Update(InDeltaTime);

	FRotator RecoilRotation = SpringRecoilTransform.GetRotation().Rotator();
	RecoilRotation.Roll -= RecoilSpring_Pitch.Update(InDeltaTime);

	/* The spring can settle in a position that isn't exactly zero. To compensate, we constantly interp recoil to zero */
	RecoilTranslation = FMath::InterpEaseOut(RecoilTranslation, FVector::ZeroVector, InDeltaTime, TranslationEaseExp);
	RecoilRotation = FMath::InterpEaseOut(RecoilRotation, FRotator::ZeroRotator, InDeltaTime, RotationEaseExp);

	SpringRecoilTransform.SetTranslation(RecoilTranslation);
	SpringRecoilTransform.SetRotation(RecoilRotation.Quaternion());
}

void UMETRecoilComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter, const EWeaponFiringMode& InFiringMode)
{
	Reset();
	OwningCharacter = InOwningCharacter;
	FiringMode = InFiringMode;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InOwningCharacter);
	if (ensure(InOwningCharacter) && InOwningCharacter->IsLocallyControlled() && ensure(ASC))
	{
		GameplayTagEventHandle_FireWeapon = ASC->RegisterGameplayTagEvent(METGameplayTags::Ability_FireWeapon).AddLambda([&](FGameplayTag InTag, int32 InCount)
		{
			if (InCount > 0)
			{
				// Fire action started
				FireActionStartTime = GetWorld()->GetTimeSeconds();
				LastRecoilCurvePos = FVector2d::ZeroVector;
				CurrentRecoilCurvePos = FVector2d::ZeroVector;
			}
		});
	}
}

void UMETRecoilComponent::OnFireActionHeld()
{
	if(AimRecoilCurve && FiringMode == Automatic)
	{
		LastRecoilCurvePos = CurrentRecoilCurvePos;
		const float ElapsedRecoilTime = GetWorld()->GetTimeSeconds() - FireActionStartTime;
		float CurveValue = AimRecoilCurve->GetFloatValue(GetWorld()->GetTimeSeconds() - FireActionStartTime);
		CurveValue = FMath::FRandRange(CurveValue - AimRecoilNoise, CurveValue + AimRecoilNoise);
		CurrentRecoilCurvePos = FVector2d(ElapsedRecoilTime, CurveValue);

		//GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("ElapsedRecoilTime: %f"), ElapsedRecoilTime));
	}
}

void UMETRecoilComponent::OnWeaponFired()
{
	if(!ensure(OwningCharacter)) return;

	AController* Controller = OwningCharacter->GetController();
	
	if(Controller && AimRecoilCurve && FiringMode == Automatic)
	{
		FRotator ControlRotation = Controller->GetControlRotation();
		FVector2d RecoilDirection = CurrentRecoilCurvePos - LastRecoilCurvePos;
		RecoilDirection.Normalize();
		
		ControlRotation += FRotator(RecoilDirection.X * VerticalAimRecoilMultiplier, -RecoilDirection.Y * HorizontalAimRecoilMultiplier, 0.f);
		Controller->SetControlRotation(ControlRotation);
	}

	RecoilSpring_Z.AddInstantaneousForce(RecoilForce_Z);
	RecoilSpring_Y.AddInstantaneousForce(RecoilForce_Y);
	RecoilSpring_Pitch.AddInstantaneousForce(RecoilForce_Pitch);
}

void UMETRecoilComponent::Reset()
{
	if (GameplayTagEventHandle_FireWeapon.IsSet())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningCharacter);
		if (ensure(ASC))
		{
			ASC->UnregisterGameplayTagEvent(GameplayTagEventHandle_FireWeapon.GetValue(), METGameplayTags::Ability_FireWeapon);
			GameplayTagEventHandle_FireWeapon.Reset();
		}
	}
	
	OwningCharacter = nullptr;
	FiringMode = SingleShot;
	FireActionStartTime = 0.f;
	CurrentRecoilCurvePos = FVector2d::ZeroVector;
	LastRecoilCurvePos = FVector2d::ZeroVector;
	SpringRecoilTransform = FTransform::Identity;
}