#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	virtual void Die();

	void MoveTowardsLocation(FVector location, float speedFactor);
	void MoveTowardsOtherAnimal(AAnimal* OtherAnimal, float speedFactor);	
	FORCEINLINE void MoveInDirection(FVector Direction, float SpeedFactor) { AddMovementInput(Direction, SpeedFactor); }

	void SetRandomTarget();
	FVector GetRandomPointWithinReach(float ReachRadius);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius, float ConeAngleDegrees);

	virtual float CalculateSpeedFromStamina();

	virtual void DrainStamina(float DeltaTime);
	virtual void RegenerateStamina(float DeltaTime);
	virtual void DrainHunger(float DeltaTime);

	virtual bool NeedRest();
	virtual bool ShouldExitResting();
	virtual bool EnoughStamina();
	virtual bool IsHungry();
	virtual bool HasStarved();

	bool HasReachedLocation(FVector Location);

	virtual void StartEating();
	virtual void StartWandering();
	virtual void StartResting();
	void MoveToTarget(AActor* Target);

protected:
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
	float HungerThreshold = 0.70f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger")
	float HungerDrainRate = 1.5f;

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

	UPROPERTY()
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
};
