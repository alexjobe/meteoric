// Copyright Alex Jobe


#include "METRecoilComponent.h"

#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"

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
	OwningCharacter = InOwningCharacter;
	FiringMode = InFiringMode;
}

void UMETRecoilComponent::OnFireActionStarted()
{
	FireActionStartTime = GetWorld()->GetTimeSeconds();
	LastRecoilCurvePos = FVector2d::ZeroVector;
	CurrentRecoilCurvePos = FVector2d::ZeroVector;
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
	
	if(AimRecoilCurve && FiringMode == Automatic)
	{
		FRotator ControlRotation = OwningCharacter->GetController()->GetControlRotation();
		FVector2d RecoilDirection = CurrentRecoilCurvePos - LastRecoilCurvePos;
		RecoilDirection.Normalize();
		
		ControlRotation += FRotator(RecoilDirection.X * VerticalAimRecoilMultiplier, -RecoilDirection.Y * HorizontalAimRecoilMultiplier, 0.f);
		OwningCharacter->GetController()->SetControlRotation(ControlRotation);
	}

	RecoilSpring_Z.AddInstantaneousForce(RecoilForce_Z);
	RecoilSpring_Y.AddInstantaneousForce(RecoilForce_Y);
	RecoilSpring_Pitch.AddInstantaneousForce(RecoilForce_Pitch);
}