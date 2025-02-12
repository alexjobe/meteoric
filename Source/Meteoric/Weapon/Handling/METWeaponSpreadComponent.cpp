// Copyright Alex Jobe


#include "METWeaponSpreadComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Meteoric/Character/METCharacter.h"

static TAutoConsoleVariable<int32> CVarDrawWeaponSpreadDebug(
	TEXT("METWeaponSpread.DrawDebug"),
	0,
	TEXT("If 1, draws the current weapon spread cone and shot direction")
);

UMETWeaponSpreadComponent::UMETWeaponSpreadComponent()
	: SpreadCooldown(.2f)
	, SpreadResetLerpSpeed(2.f)
	, CurrentYawInDegrees(0.f)
	, CurrentPitchInDegrees(0.f)
	, ElapsedTimeSinceFired(0.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETWeaponSpreadComponent::InitializeComponent()
{
	Super::InitializeComponent();
	WeaponMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();

	CurrentConeSettings = DefaultConeSettings;
	ResetCurrentCone();
}

void UMETWeaponSpreadComponent::UpdateWeaponSpread(const float InDeltaTime)
{
	if (CurrentYawInDegrees == CurrentConeSettings.MinYawInDegrees && CurrentPitchInDegrees == CurrentConeSettings.MinPitchInDegrees) return;

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarDrawWeaponSpreadDebug.GetValueOnAnyThread() == 1)
	{
		DrawSpreadDebugCone();
	}
#endif
	
	ElapsedTimeSinceFired += InDeltaTime;
	if (ElapsedTimeSinceFired > SpreadCooldown)
	{
		CurrentYawInDegrees = FMath::Clamp(FMath::Lerp(CurrentYawInDegrees, CurrentConeSettings.MinYawInDegrees, InDeltaTime * SpreadResetLerpSpeed), CurrentConeSettings.MinYawInDegrees, CurrentConeSettings.MaxYawInDegrees);
		CurrentPitchInDegrees = FMath::Clamp(FMath::Lerp(CurrentPitchInDegrees, CurrentConeSettings.MinPitchInDegrees, InDeltaTime * SpreadResetLerpSpeed), CurrentConeSettings.MinPitchInDegrees, CurrentConeSettings.MaxPitchInDegrees);

		constexpr float ErrorTolerance = 0.1f;
		if (FMath::IsNearlyEqual(CurrentYawInDegrees, CurrentConeSettings.MinYawInDegrees, ErrorTolerance))
		{
			CurrentYawInDegrees = CurrentConeSettings.MinYawInDegrees;
		}

		if (FMath::IsNearlyEqual(CurrentPitchInDegrees, CurrentConeSettings.MinPitchInDegrees, ErrorTolerance))
		{
			CurrentPitchInDegrees = CurrentConeSettings.MinPitchInDegrees;
		}
	}
}

FTransform UMETWeaponSpreadComponent::GetProjectileSpawnTransform() const
{
	if (!ensure(OwningCharacter) || !ensure(WeaponMesh)) return FTransform::Identity;
	
	const FVector StartLocation = WeaponMesh->GetSocketLocation(FName("S_Muzzle"));
	const FVector ShotDirection = GetShotDirection(StartLocation);
	const FRotator StartRotation = UKismetMathLibrary::MakeRotFromX(ShotDirection);

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarDrawWeaponSpreadDebug.GetValueOnAnyThread() == 1)
	{
		const FVector EndLocation = OwningCharacter->GetFocalPoint();
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);
	}
#endif

	return FTransform(StartRotation, StartLocation);
}

FVector UMETWeaponSpreadComponent::GetShotDirection(const FVector& InStartLocation) const
{
	const FVector TraceEndLocation = OwningCharacter->GetFocalPoint();
	FVector ShotDirection = TraceEndLocation - InStartLocation;

	if (CurrentYawInDegrees > 0.f && CurrentPitchInDegrees > 0.f)
	{
		// Apply spread
		ShotDirection = UKismetMathLibrary::RandomUnitVectorInEllipticalConeInDegrees(ShotDirection, CurrentYawInDegrees, CurrentPitchInDegrees);
	}

	return ShotDirection;
}

void UMETWeaponSpreadComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	OwningCharacter = Cast<AMETCharacter>(InOwningCharacter);
	CurrentConeSettings = DefaultConeSettings;
	ResetCurrentCone();
}

void UMETWeaponSpreadComponent::OnWeaponUnequipped()
{
	OwningCharacter = nullptr;
	CurrentConeSettings = DefaultConeSettings;
	ResetCurrentCone();
}

void UMETWeaponSpreadComponent::OnWeaponFired()
{
	ElapsedTimeSinceFired = 0.f;
	CurrentYawInDegrees = FMath::Min(CurrentConeSettings.MaxYawInDegrees, CurrentYawInDegrees + CurrentConeSettings.YawIncreasePerShot);
	CurrentPitchInDegrees = FMath::Min(CurrentConeSettings.MaxPitchInDegrees, CurrentPitchInDegrees + CurrentConeSettings.PitchIncreasePerShot);
}

void UMETWeaponSpreadComponent::OnAimDownSights(const bool bInIsAiming)
{
	if (bInIsAiming)
	{
		CurrentConeSettings = AimConeSettings;
	}
	else
	{
		CurrentConeSettings = DefaultConeSettings;
	}
}

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
void UMETWeaponSpreadComponent::DrawSpreadDebugCone() const
{
	const FVector StartLocation = WeaponMesh->GetSocketLocation(FName("S_Muzzle"));
	const FVector TraceEndLocation = OwningCharacter->GetFocalPoint();
	const FVector ShotDirection = TraceEndLocation - StartLocation;
	
	DrawDebugCone(
		GetWorld(),
		StartLocation,
		ShotDirection,
		500.f,
		FMath::DegreesToRadians(CurrentYawInDegrees),
		FMath::DegreesToRadians(CurrentPitchInDegrees),
		20,
		FColor::Green
	);
}
#endif
