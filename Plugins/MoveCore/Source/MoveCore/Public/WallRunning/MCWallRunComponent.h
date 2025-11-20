// Copyright Alex Jobe

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MCWallRunComponent.generated.h"


UENUM()
enum class EMCWallSide : uint8
{
	Left,
	Right,
};

UENUM()
enum class EMCWallRunEndReason : uint8
{
	FellOffWall,
	JumpedOffWall
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOVECORE_API UMCWallRunComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning")
	float WallRunGravityScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning")
	float WallRunAirControl;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (ToolTip = "How much should the character lean away from the wall while wall running?"))
	float WallRunLean;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (ToolTip = "How fast should the character lean away from the wall while wall running?"))
	float WallRunLeanSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning", meta = (ToolTip = "How long should the character keep wall running once wall run keys are no longer held down?"))
	float TimeToFallOff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning")
	float LineTraceDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WallRunning")
	TEnumAsByte<ECollisionChannel> LineTraceChannel;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta=(ClampMin="0"))
	float InputDeadZone;
	
	UMCWallRunComponent();

	//~ Begin UActorComponent interface
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

private:
	UPROPERTY(Transient);
	TObjectPtr<ACharacter> OwningCharacter;

	UPROPERTY(Transient)
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	bool bIsWallRunning;

	EMCWallSide WallSide;
	FVector WallRunDirection;

	float ForwardInput;
	float RightInput;

	float DefaultGravityScale;
	float DefaultAirControl;

	float ElapsedFallOffTime;

	/* Called when we collide with something, so we can check if we hit a wall we can run on */
	UFUNCTION()
	void OwningCharacter_OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	/* Returns true if we can wall run on a given surface, provided the surface normal */
	bool CanWallRunOnSurface(const FVector& InSurfaceNormal) const;

	/* Find out which side the wall is on, based on its surface normal */
	EMCWallSide FindWallSide(const FVector& InSurfaceNormal) const;

	static FVector FindRunDirection(const FVector& InSurfaceNormal, const EMCWallSide& InSide);
	
	bool IsWallRunInputActive() const;

	void BeginWallRun();

	void WallRunUpdate(const float InDeltaTime);

	bool LineTraceToWall(FHitResult& InHitResult) const;

	void EndWallRun(const EMCWallRunEndReason& InEndReason);
	
	void SetIsWallRunning(const bool InIsWallRunning);

	void MoveInput(const struct FInputActionValue& Value);

public:
	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent);
};
