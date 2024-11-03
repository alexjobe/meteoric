#include "METSpring.h"

FMETSpring::FMETSpring()
	: SpringConstant(100.f)
	, DampingRatio(0.5f)
	, DampedAngularFrequency(0)
	, InitialVelocity(0)
	, CurrentVelocity(0)
	, InitialDisplacement(0)
	, CurrentDisplacement(0)
	, ElapsedTime(0)
{
	DampedAngularFrequency = FMath::Sqrt(SpringConstant - FMath::Square(DampingRatio));
}

void FMETSpring::ApplyInstantaneousForce(float InForce)
{
	InitialVelocity = CurrentVelocity + InForce;
	InitialDisplacement = CurrentDisplacement;
	ElapsedTime = 0.f;
}

float FMETSpring::UpdateSpring(float InDeltaTime)
{
	ElapsedTime += InDeltaTime;
	
	const float A = InitialDisplacement;
	const float B = (InitialVelocity + DampingRatio * InitialDisplacement) / DampedAngularFrequency;
	constexpr float e = 2.71828f;

	const float C = (B * DampedAngularFrequency - A * DampingRatio) * FMath::Cos(DampedAngularFrequency * ElapsedTime)
	- (A * DampedAngularFrequency + B * DampingRatio) * FMath::Sin(DampedAngularFrequency * ElapsedTime);

	CurrentVelocity = FMath::Pow(e, -DampingRatio * ElapsedTime) * C;

	CurrentDisplacement = CurrentDisplacement + CurrentVelocity * InDeltaTime;

	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, FString::Printf(TEXT("SpringRecoilCurrentDisplacement: %f"), CurrentDisplacement));

	return CurrentDisplacement;
}
