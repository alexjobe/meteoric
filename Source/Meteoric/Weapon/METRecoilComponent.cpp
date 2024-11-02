// Copyright Alex Jobe


#include "METRecoilComponent.h"

#include "Curves/CurveFloat.h"
#include "GameFramework/Character.h"

UMETRecoilComponent::UMETRecoilComponent()
	: RecoilCurve(nullptr)
	, VerticalRecoilMultiplier(1.f)
	, HorizontalRecoilMultiplier(1.f)
	, RecoilNoise(0.01f)
	, FireActionStartTime(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMETRecoilComponent::OnWeaponEquipped(ACharacter* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
}

void UMETRecoilComponent::OnFireActionStarted()
{
	FireActionStartTime = GetWorld()->GetTimeSeconds();
	LastRecoilCurvePos = FVector2d::ZeroVector;
	CurrentRecoilCurvePos = FVector2d::ZeroVector;
}

void UMETRecoilComponent::OnFireActionHeld()
{
	if(RecoilCurve)
	{
		LastRecoilCurvePos = CurrentRecoilCurvePos;
		const float ElapsedRecoilTime = GetWorld()->GetTimeSeconds() - FireActionStartTime;
		float CurveValue = RecoilCurve->GetFloatValue(GetWorld()->GetTimeSeconds() - FireActionStartTime);
		CurveValue = FMath::FRandRange(CurveValue - RecoilNoise, CurveValue + RecoilNoise);
		CurrentRecoilCurvePos = FVector2d(ElapsedRecoilTime, CurveValue);

		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("ElapsedRecoilTime: %f"), ElapsedRecoilTime));
	}
}

void UMETRecoilComponent::OnWeaponFired()
{
	if(!ensure(OwningCharacter)) return;
	
	if(RecoilCurve)
	{
		FRotator ControlRotation = OwningCharacter->GetController()->GetControlRotation();
		FVector2d RecoilDirection = CurrentRecoilCurvePos - LastRecoilCurvePos;
		RecoilDirection.Normalize();
		
		ControlRotation += FRotator(RecoilDirection.X * VerticalRecoilMultiplier, -RecoilDirection.Y * HorizontalRecoilMultiplier, 0.f);
		OwningCharacter->GetController()->SetControlRotation(ControlRotation);
	}
}
