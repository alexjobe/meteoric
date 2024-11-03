// Copyright Alex Jobe


#include "METRecoilComponent.h"

#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"

UMETRecoilComponent::UMETRecoilComponent()
	: AimRecoilCurve(nullptr)
	, VerticalAimRecoilMultiplier(1.f)
	, HorizontalAimRecoilMultiplier(1.f)
	, AimRecoilNoise(0.01f)
	, FiringMode(SingleShot)
	, FireActionStartTime(0.f)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UMETRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	RecoilSpring_Z.Initialize();
	RecoilSpring_Y.Initialize();
	RecoilSpring_Pitch.Initialize();
}

void UMETRecoilComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RecoilSpring_Z.UpdateSpring(DeltaTime);
	RecoilSpring_Y.UpdateSpring(DeltaTime);
	RecoilSpring_Pitch.UpdateSpring(DeltaTime);
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

	RecoilSpring_Z.AddInstantaneousForce(100.f);
	RecoilSpring_Y.AddInstantaneousForce(200.f);
	RecoilSpring_Pitch.AddInstantaneousForce(100.f);
}