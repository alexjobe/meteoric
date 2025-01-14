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
	: AimTraceRange(3000.f)
	, SpreadCooldown(.2f)
	, MinYawInDegrees(2.f)
	, MinPitchInDegrees(2.f)
	, MaxYawInDegrees(5.f)
	, MaxPitchInDegrees(5.f)
	, YawIncreasePerShot(0.5f)
	, PitchIncreasePerShot(0.5f)
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

	CurrentYawInDegrees = MinYawInDegrees;
	CurrentPitchInDegrees = MinPitchInDegrees;
}

void UMETWeaponSpreadComponent::UpdateWeaponSpread(const float InDeltaTime)
{
	if (CurrentYawInDegrees == MinYawInDegrees && CurrentPitchInDegrees == MinPitchInDegrees) return;

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
	if (CVarDrawWeaponSpreadDebug.GetValueOnAnyThread() == 1)
	{
		DrawSpreadDebugCone();
	}
#endif
	
	ElapsedTimeSinceFired += InDeltaTime;
	if (ElapsedTimeSinceFired > SpreadCooldown)
	{
		CurrentYawInDegrees = FMath::Max(MinYawInDegrees, FMath::Lerp(CurrentYawInDegrees, MinYawInDegrees, InDeltaTime * SpreadResetLerpSpeed));
		CurrentPitchInDegrees = FMath::Max(MinPitchInDegrees, FMath::Lerp(CurrentPitchInDegrees, MinPitchInDegrees, InDeltaTime * SpreadResetLerpSpeed));
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
		const FVector EndLocation = StartLocation + ShotDirection * AimTraceRange;
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);
	}
#endif

	return FTransform(StartRotation, StartLocation);
}

FVector UMETWeaponSpreadComponent::GetShotDirection(const FVector& InStartLocation) const
{
	const FTransform EyesViewpoint = OwningCharacter->GetEyesViewpoint();
	const FVector TraceEndLocation = EyesViewpoint.GetLocation() + EyesViewpoint.GetRotation().GetForwardVector() * AimTraceRange;

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
	CurrentYawInDegrees = MinYawInDegrees;
	CurrentPitchInDegrees = MinPitchInDegrees;
}

void UMETWeaponSpreadComponent::OnWeaponUnequipped()
{
	OwningCharacter = nullptr;
	CurrentYawInDegrees = MinYawInDegrees;
	CurrentPitchInDegrees = MinPitchInDegrees;
}

void UMETWeaponSpreadComponent::OnWeaponFired()
{
	ElapsedTimeSinceFired = 0.f;
	CurrentYawInDegrees = FMath::Min(MaxYawInDegrees, CurrentYawInDegrees + YawIncreasePerShot);
	CurrentPitchInDegrees = FMath::Min(MaxPitchInDegrees, CurrentPitchInDegrees + PitchIncreasePerShot);
}

#if !UE_BUILD_TEST && !UE_BUILD_SHIPPING
void UMETWeaponSpreadComponent::DrawSpreadDebugCone() const
{
	const FVector StartLocation = WeaponMesh->GetSocketLocation(FName("S_Muzzle"));
	const FTransform EyesViewpoint = OwningCharacter->GetEyesViewpoint();
	const FVector TraceEndLocation = EyesViewpoint.GetLocation() + EyesViewpoint.GetRotation().GetForwardVector() * AimTraceRange;
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
