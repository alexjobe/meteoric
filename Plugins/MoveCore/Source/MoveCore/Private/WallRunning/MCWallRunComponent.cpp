// Copyright Alex Jobe


#include "WallRunning/MCWallRunComponent.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


UMCWallRunComponent::UMCWallRunComponent()
	: WallRunGravityScale(0)
	, WallRunAirControl(1.f)
	, WallRunLean(20.f)
	, WallRunLeanSpeed(10.f)
	, WallRunCooldown(0.5f)
	, TimeToFallOff(0.5f)
	, LineTraceDistance(200.f)
	, LineTraceChannel(ECC_Visibility)
	, InputDeadZone(0.1f)
	, bIsWallRunning(false)
	, WallSide(EMCWallSide::Right)
	, WallRunDirection(FVector::ZeroVector)
	, ForwardInput(0)
	, RightInput(0)
	, DefaultGravityScale(0)
	, DefaultAirControl(0)
	, WallRunCooldownRemaining(0)
	, ElapsedFallOffTime(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMCWallRunComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter = Cast<ACharacter>(GetOwner());
	check(OwningCharacter);

	MovementComponent = OwningCharacter->GetCharacterMovement();
	check(MovementComponent);

	// Needed so we can "stick" character to wall
	MovementComponent->SetPlaneConstraintEnabled(true);

	// These values are changed when wall running, so we save them to change back when wall run ends
	DefaultGravityScale = MovementComponent->GravityScale;
	DefaultAirControl = MovementComponent->AirControl;

	OwningCharacter->OnActorHit.AddUniqueDynamic(this, &UMCWallRunComponent::OwningCharacter_OnActorHit);
}

void UMCWallRunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (WallRunCooldownRemaining > 0)
	{
		WallRunCooldownRemaining = FMath::Max(WallRunCooldownRemaining - DeltaTime, 0.f);
	}
	if (bIsWallRunning)
	{
		WallRunUpdate(DeltaTime);
	}
	CharacterLeanUpdate(DeltaTime);
}

void UMCWallRunComponent::OwningCharacter_OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!MovementComponent) return;
	// Make sure we are not grounded or already wall running
	if (bIsWallRunning || !MovementComponent->IsFalling() || WallRunCooldownRemaining > 0) return;

	if (CanWallRunOnSurface(Hit.ImpactNormal))
	{
		WallSide = FindWallSide(Hit.ImpactNormal);
		if (IsWallRunInputActive())
		{
			WallRunDirection = FindRunDirection(Hit.ImpactNormal, WallSide);
			BeginWallRun();
		}
	}
}

bool UMCWallRunComponent::CanWallRunOnSurface(const FVector& InSurfaceNormal) const
{
	if (!MovementComponent) return false;
	// Cannot wall run on ceilings or surfaces that we can walk on
	return InSurfaceNormal.Z > -0.05f && InSurfaceNormal.Z < MovementComponent->GetWalkableFloorZ();
}

EMCWallSide UMCWallRunComponent::FindWallSide(const FVector& InSurfaceNormal) const
{
	if (OwningCharacter)
	{
		const float DotProductResult = FVector2D::DotProduct(FVector2D(InSurfaceNormal), FVector2D(OwningCharacter->GetActorRightVector()));
		// If the character's right vector is in the same direction as the wall's surface normal, the wall is on the left.
		if (DotProductResult > 0) return EMCWallSide::Left;
	}
	// Otherwise, the wall is on the right.
	return EMCWallSide::Right;
}

FVector UMCWallRunComponent::FindRunDirection(const FVector& InSurfaceNormal, const EMCWallSide& InSide)
{
	switch (InSide)
	{
	case EMCWallSide::Left:
		return FVector::CrossProduct(InSurfaceNormal, FVector(0.f, 0.f, 1.f));
	case EMCWallSide::Right:
		return FVector::CrossProduct(InSurfaceNormal, FVector(0.f, 0.f, -1.f));
	}
	return FVector::ZeroVector;
}

bool UMCWallRunComponent::IsWallRunInputActive() const
{
	if (!OwningCharacter) return false;

	// Must be running forward to wall run
	if (ForwardInput < FMath::Abs(InputDeadZone))
		return false;

	switch (WallSide)
	{
	case EMCWallSide::Left:
		return RightInput < -FMath::Abs(InputDeadZone);
	case EMCWallSide::Right:
		return RightInput > FMath::Abs(InputDeadZone);
	default:
		return false;
	}
}

void UMCWallRunComponent::BeginWallRun()
{
	if (!MovementComponent) return;
	
	// Restrict movement on the Z axis 
	MovementComponent->SetPlaneConstraintNormal({0.0f, 0.0f, 1.0f});
	// Restrict rotation while wall running
	MovementComponent->bOrientRotationToMovement = false;
	MovementComponent->GravityScale = WallRunGravityScale;
	MovementComponent->AirControl = WallRunAirControl;
	ElapsedFallOffTime = 0.f;
	SetIsWallRunning(true);
}

void UMCWallRunComponent::WallRunUpdate(const float InDeltaTime)
{
	if (!MovementComponent) return;
	
	// If the required input is not held down, we start a timer to determine if we should fall off. If the input is still 
	// not held down when the timer completes, we fall off the wall. This gives the player time to correct the mistake.
	if (!IsWallRunInputActive())
	{
		ElapsedFallOffTime += InDeltaTime;
	}
	else
	{
		ElapsedFallOffTime = 0.f;
	}

	if (ElapsedFallOffTime >= TimeToFallOff)
	{
		EndWallRun(EMCWallRunEndReason::FellOffWall);
		return;
	}

	FHitResult HitResult;
	const bool bHitWall = LineTraceToWall(HitResult);

	// If line trace returns false, fall off wall
	if (!bHitWall)
	{
		EndWallRun(EMCWallRunEndReason::FellOffWall);
		return;
	}
	
	// If wall side has changed for some reason, fall off wall
	if (WallSide != FindWallSide(HitResult.ImpactNormal))
	{
		EndWallRun(EMCWallRunEndReason::FellOffWall);
		return;
	}

	WallRunDirection = FindRunDirection(HitResult.ImpactNormal, WallSide);

	MovementComponent->Velocity.X = MovementComponent->GetMaxSpeed() * WallRunDirection.X;
	MovementComponent->Velocity.Y = MovementComponent->GetMaxSpeed() * WallRunDirection.Y;
}

bool UMCWallRunComponent::LineTraceToWall(FHitResult& InHitResult) const
{
	if (!OwningCharacter) return false;
	FVector LineTraceEnd;
	
	switch (WallSide)
	{
	case EMCWallSide::Left:
		LineTraceEnd = FVector::CrossProduct(WallRunDirection, FVector(0.f, 0.f, 1.f));
		break;
	case EMCWallSide::Right:
		LineTraceEnd = FVector::CrossProduct(WallRunDirection, FVector(0.f, 0.f, -1.f));
		break;
	}

	LineTraceEnd *= LineTraceDistance;
	LineTraceEnd += OwningCharacter->GetActorLocation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(OwningCharacter);

	return GetWorld()->LineTraceSingleByChannel(InHitResult, OwningCharacter->GetActorLocation(), LineTraceEnd,
		LineTraceChannel, CollisionParams);
}

void UMCWallRunComponent::EndWallRun(const EMCWallRunEndReason& InEndReason)
{
	if(!OwningCharacter || !MovementComponent) return;
	
	//if(InEndReason == EMETWallRunEndReason::FellOffWall) SetJumps(JumpsLeftAfterFalling);
	MovementComponent->SetPlaneConstraintNormal({ 0.f, 0.f, 0.f });
	MovementComponent->GravityScale = DefaultGravityScale;
	MovementComponent->AirControl = DefaultAirControl;
	WallRunCooldownRemaining = WallRunCooldown;
	SetIsWallRunning(false);
}

void UMCWallRunComponent::CharacterLeanUpdate(const float InDeltaTime) const
{
	if(!OwningCharacter) return;

	const FRotator CurrentRotation = OwningCharacter->GetActorRotation();
	
	// If we aren't wall running and the character roll is already zero, we don't need to do anything
	if (!bIsWallRunning && CurrentRotation.Roll == 0.f) return;
	
	UE_LOG(LogTemp, Warning, TEXT("CurrentCharacterRoll: %f"), CurrentRotation.Roll)

	// Get target character roll, based on whether we are wall running and which side of the wall we are on
	FRotator TargetRotation = { CurrentRotation.Pitch, CurrentRotation.Yaw, GetTargetCharacterRoll() };

	if (bIsWallRunning)
	{
		// If we are wall running, we want to be rotated parallel to the wall
		TargetRotation.Pitch = WallRunDirection.Rotation().Pitch;
		TargetRotation.Yaw = WallRunDirection.Rotation().Yaw;
	}

	// Smoothly interpolate towards target character roll
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, InDeltaTime, WallRunLeanSpeed);

	if (!bIsWallRunning && FMath::IsNearlyZero(NewRotation.Roll, 0.01f))
	{
		MovementComponent->bOrientRotationToMovement = true;
		NewRotation.Roll = 0.0f;
	}
	
	OwningCharacter->SetActorRotation(NewRotation);
}

float UMCWallRunComponent::GetTargetCharacterRoll() const
{
	if (bIsWallRunning)
	{
		if (WallSide == EMCWallSide::Left) return WallRunLean;
		if (WallSide == EMCWallSide::Right) return -WallRunLean;
	}
	// If we aren't wall running, we want to tilt the character back to normal
	return 0.f;
}

void UMCWallRunComponent::SetIsWallRunning(const bool InIsWallRunning)
{
	bIsWallRunning = InIsWallRunning;
}

void UMCWallRunComponent::MoveInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	ForwardInput = MovementVector.Y;
	RightInput = MovementVector.X;
}

void UMCWallRunComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MoveInput);
	}
}

