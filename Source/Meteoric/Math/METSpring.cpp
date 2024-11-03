#include "METSpring.h"

FMETSpring::FMETSpring()
	: SpringConstant(75.f)
	, Mass(1.f)
	, bCriticallyDamped(true)
	, DampingConstant(0.1f)
	, DampingRatio(5.f)
	, DampedAngularFrequency(.5f)
	, ImpulseVelocity(0)
	, CurrentVelocity(0)
	, InitialDisplacement(0)
	, CurrentDisplacement(0)
	, ElapsedTime(0)
{
	Initialize();
}

void FMETSpring::Initialize()
{
	if(!ensure(Mass > 0.f && SpringConstant > 0.f))
	{
		UE_LOG(LogTemp, Error, TEXT("METSpring: Mass and SpringConstant must be greater than zero"));
	}
	
	const float CriticallyDampedCoefficient = FMath::Sqrt(4.f * Mass * SpringConstant) - 0.1f;

	if(bCriticallyDamped)
	{
		DampingConstant = CriticallyDampedCoefficient;
	}
	else
	{
		if(DampingConstant > CriticallyDampedCoefficient)
		{
			UE_LOG(LogTemp, Error, TEXT("METSpring: DampingConstant should not exceed sqrt(4 * m * k) == %f"), CriticallyDampedCoefficient);
		}
		DampingConstant = FMath::Clamp(DampingConstant, 0.f, CriticallyDampedCoefficient);
	}
	
	DampingRatio = DampingConstant / (2.f * Mass);
	const float SpringConstantMassRatio = SpringConstant / Mass;
	const float DampingRatioSquared = FMath::Square(DampingRatio);
	
	if(ensure(DampingRatioSquared < SpringConstantMassRatio))
	{
		DampedAngularFrequency = FMath::Sqrt(SpringConstantMassRatio - DampingRatioSquared);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("METSpring: DampingRatioSquared >= SpringConstantMassRatio"));
	}
}

void FMETSpring::AddInstantaneousForce(float InForce, float InDeltaTime)
{
	ImpulseVelocity = CurrentVelocity + (InForce * InDeltaTime) / Mass;
	InitialDisplacement = CurrentDisplacement;
	ElapsedTime = 0.f;
}

float FMETSpring::UpdateSpring(float InDeltaTime)
{
	if(ImpulseVelocity == 0.f) return 0.f;
	
	ElapsedTime += InDeltaTime;
	
	const float A = InitialDisplacement;
	const float B = (ImpulseVelocity + DampingRatio * InitialDisplacement) / DampedAngularFrequency;
	constexpr float e = 2.71828f;

	const float C = (B * DampedAngularFrequency - A * DampingRatio) * FMath::Cos(DampedAngularFrequency * ElapsedTime)
	- (A * DampedAngularFrequency + B * DampingRatio) * FMath::Sin(DampedAngularFrequency * ElapsedTime);

	CurrentVelocity = FMath::Pow(e, -DampingRatio * ElapsedTime) * C;

	CurrentDisplacement = CurrentDisplacement + CurrentVelocity * InDeltaTime;

	GEngine->AddOnScreenDebugMessage(1, .2f, FColor::Blue, FString::Printf(TEXT("SpringRecoilCurrentDisplacement: %f"), CurrentDisplacement));

	DisplacementQueue.Enqueue(FMath::Abs(CurrentDisplacement));
	DisplacementQueueSize++;
	TotalDisplacement += FMath::Abs(CurrentDisplacement);

	constexpr int MaxQueueSize = 30;

	if(DisplacementQueueSize > MaxQueueSize)
	{
		float OldestDisplacement = 0.f;
		DisplacementQueue.Dequeue(OldestDisplacement);
		DisplacementQueueSize--;
		TotalDisplacement -= OldestDisplacement;

		if(TotalDisplacement < .1f * DisplacementQueueSize)
		{
			ImpulseVelocity = 0.f;
			CurrentVelocity = 0.f;
			InitialDisplacement = 0.f;
			CurrentDisplacement = 0.f;
			ElapsedTime = 0.f;
			DisplacementQueue.Empty();
			DisplacementQueueSize = 0;
		}
	}

	return CurrentDisplacement;
}

FMETSpring& FMETSpring::operator=(const FMETSpring& Other)
{
	SpringConstant = Other.SpringConstant;
	Mass = Other.Mass;
	bCriticallyDamped = Other.bCriticallyDamped;
	DampingConstant = Other.DampingConstant;
	DampingRatio = Other.DampingRatio;
	DampedAngularFrequency = Other.DampedAngularFrequency;
	return *this;
}
