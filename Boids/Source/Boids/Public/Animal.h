#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AnimalStates.h"
#include "AnimalTypes.h"
#include "Animal.generated.h"  // This should be the last include

class AAIController;

UCLASS()
class BOIDS_API AAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	
	AAnimal();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetDeadState();
	virtual void Die();

	void MoveTowardsLocation(FVector DesiredLocation, float SpeedFactor);
	void MoveTowardsOtherAnimal(AAnimal* OtherAnimal, float SpeedFactor);	

	void SetRandomTarget();
	FVector GetRandomPointWithinReach(float ReachRadius);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius, float ConeAngleDegrees);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius, float ConeAngleDegrees, bool bBehind);
	FVector GetRandomPointNear(FVector TargetLocation, float MinReachRadius, float MaxReachRadius);
	bool IsPathBlocked(FVector TargetLocation);

	virtual float CalculateSpeedFromStamina();

	virtual void DrainStamina(float DeltaTime);
	virtual void RegenerateStamina(float DeltaTime);
	virtual void DrainHunger(float DeltaTime);
	virtual void RegenerateHunger(float DeltaTime);

	virtual bool NeedRest();
	virtual bool ShouldExitResting();
	virtual bool EnoughStamina();
	virtual bool IsHungry();
	virtual bool HasStarved();
	virtual bool IsStuck();

	bool HasReachedLocation(FVector Location);

	virtual void StartEating();
	virtual void StartWandering();
	virtual void StartResting();
	void MoveToTarget(AActor* Target);

	

protected:

	FVector LastCheckedLocation;
	float LastCheckTime;
	const float StuckThresholdDistance = 10.0f; // Threshold distance to determine if stuck
	const float CheckInterval = 0.1f;           // Interval in seconds to check if stuck
	
	// --- Stamina variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina")
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina")
	float MaxStamina = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (ClampMin = "0.0", ClampMax = "1.0",
		ToolTip = "Stamina threshold for transitioning to resting state"))
	float StaminaRestThreshold = 0.30f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (
		ToolTip = "Rate at which stamina regenerates when resting"))
	float StaminaRegenRate = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina")
	float StaminaDrainRate = 2.0f;

	// --- Hunger variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger")
	float Hunger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger")
	float MaxHunger = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger", meta = (ClampMin = "0.0", ClampMax = "1.0",
		ToolTip = "The percentage of MaxHunger below which the animal will start to look for food."))
	float HungerThreshold = 0.85f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger")
	float HungerDrainRate = 1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger")
	float HungerRegenRate = 5.0f;

	// --- Speed variables ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Speed")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Speed")
	float MinWalkingSpeed = 0.10f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Speed")
	float MaxWalkingSpeed = 0.30f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Speed")
	float MaxSprintSpeed = 0.80f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Vision")
	float SightRadius = 1500.0f;

	// --- Navigation variables ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Info")
	EAnimalState AnimalState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Info")
	EAnimalType AnimalType;

	UPROPERTY(EditAnywhere, Category = "AI | Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI | Navigation")
	float AcceptanceRadius = 100.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug Mode")
	bool bDebugMode = false;

	AAIController* AnimalController;
	FTimerHandle TargetChangeTimer;
	FTimerHandle EatTimer;

	// Public section for getters
public:
	
	FORCEINLINE EAnimalState GetAnimalState() const { return AnimalState; }
	FORCEINLINE EAnimalType GetAnimalType() const { return AnimalType; }
	FORCEINLINE AAIController* GetAnimalController() const { return AnimalController; }
	FORCEINLINE FVector GetAnimalPosition() const { return GetActorLocation(); }
	FORCEINLINE FVector GetAnimalVelocity() const { return GetCharacterMovement()->Velocity; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE void DecreaseStamina() { Stamina -= 0.1f;  }
	FORCEINLINE void IncreaseStamina() { Stamina += 0.1f; }
	FORCEINLINE void DecreaseHunger() { Hunger -= 0.1f; }
	FORCEINLINE void IncreaseHunger() { Hunger += 0.1f; }
	FORCEINLINE void MoveInDirection(FVector Direction, float SpeedFactor) { AddMovementInput(Direction, SpeedFactor); }
	
};
