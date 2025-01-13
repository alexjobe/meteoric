// Copyright Alex Jobe


#include "METWeaponSpreadComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Meteoric/Character/METCharacter.h"

UMETWeaponSpreadComponent::UMETWeaponSpreadComponent()
	: TraceRange(3000.f)
	, MaxYawInDegrees(5.f)
	, MaxPitchInDegrees(5.f)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UMETWeaponSpreadComponent::InitializeComponent()
{
	Super::InitializeComponent();
	WeaponMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
}

FTransform UMETWeaponSpreadComponent::GetProjectileSpawnTransform() const
{
	if (!ensure(OwningCharacter) || !ensure(WeaponMesh)) return FTransform::Identity;
	
	const FVector StartLocation = WeaponMesh->GetSocketLocation(FName("S_Muzzle"));
	const FVector ShotDirection = GetShotDirection(StartLocation);
	const FRotator StartRotation = UKismetMathLibrary::MakeRotFromX(ShotDirection);

	const FVector EndLocation = StartLocation + ShotDirection * TraceRange;

	DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 10.f, 0, 1.f);

	DrawDebugCone(GetWorld(), StartLocation, ShotDirection, 500.f, FMath::DegreesToRadians(MaxYawInDegrees), FMath::DegreesToRadians(MaxPitchInDegrees), 20, FColor::Green, false, 10.f);

	return FTransform(StartRotation, StartLocation);
}

FVector UMETWeaponSpreadComponent::GetShotDirection(const FVector& InStartLocation) const
{
	const FTransform EyesViewpoint = OwningCharacter->GetEyesViewpoint();
	const FVector TraceEndLocation = EyesViewpoint.GetLocation() + EyesViewpoint.GetRotation().GetForwardVector() * TraceRange;

	FVector ShotDirection = TraceEndLocation - InStartLocation;

	if (MaxYawInDegrees > 0.f && MaxPitchInDegrees > 0.f)
	{
		// Apply spread
		ShotDirection = UKismetMathLibrary::RandomUnitVectorInEllipticalConeInDegrees(ShotDirection, MaxYawInDegrees, MaxPitchInDegrees);
	}

	return ShotDirection;
}

void UMETWeaponSpreadComponent::OnWeaponEquipped(ACharacter* const InOwningCharacter)
{
	OwningCharacter = Cast<AMETCharacter>(InOwningCharacter);
}

void UMETWeaponSpreadComponent::OnWeaponUnequipped()
{
	OwningCharacter = nullptr;
}
