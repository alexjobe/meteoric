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

	SpringConstant = 100.f;
	DampingRatio = 0.5f;
	DampedAngularFrequency = FMath::Sqrt(SpringConstant - FMath::Square(DampingRatio));
}

void UMETRecoilComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateSpringRecoil(DeltaTime);
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

	SpringRecoilInitialVelocity = SpringRecoilCurrentVelocity + 100.f;
	SpringRecoilInitialDisplacement = SpringRecoilCurrentDisplacement;
	SpringRecoilElapsedTime = 0.f;
}

void UMETRecoilComponent::UpdateSpringRecoil(float DeltaTime)
{
	if(SpringRecoilInitialVelocity == 0.f) return;
	
	SpringRecoilElapsedTime += DeltaTime;
	
	const float A = SpringRecoilInitialDisplacement;
	const float B = (SpringRecoilInitialVelocity + DampingRatio * SpringRecoilInitialDisplacement) / DampedAngularFrequency;
	constexpr float e = 2.71828f;

	const float C = (B * DampedAngularFrequency - A * DampingRatio) * FMath::Cos(DampedAngularFrequency * SpringRecoilElapsedTime)
	- (A * DampedAngularFrequency + B * DampingRatio) * FMath::Sin(DampedAngularFrequency * SpringRecoilElapsedTime);

	SpringRecoilCurrentVelocity = FMath::Pow(e, -DampingRatio * SpringRecoilElapsedTime) * C;

	SpringRecoilCurrentDisplacement = SpringRecoilCurrentDisplacement + SpringRecoilCurrentVelocity * DeltaTime;

	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("SpringRecoilCurrentDisplacement: %f"), SpringRecoilCurrentDisplacement));
}
