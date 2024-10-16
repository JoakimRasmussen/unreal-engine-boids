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

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Die();

	void MoveTowardsLocation(FVector location);
	void MoveTowardsLocation(FVector location, float speed);

	FORCEINLINE void MoveInDirection(FVector Direction, float SpeedFactor) { AddMovementInput(Direction, SpeedFactor); }

	// Public section for getters
public:
	FORCEINLINE EAnimalState GetAnimalState() const { return AnimalState; }
	FORCEINLINE EAnimalType GetAnimalType() const { return AnimalType; }

	FORCEINLINE AAIController* GetAnimalController() const { return AnimalController; }

	FORCEINLINE FVector GetAnimalPosition() const { return GetActorLocation(); }
	FORCEINLINE FVector GetAnimalVelocity() const { return GetCharacterMovement()->Velocity; }

protected:
	virtual void BeginPlay() override;

	void SetRandomTarget();
	FVector GetRandomPointWithinReach(float ReachRadius);
	FVector GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius);
	void StartEating();
	void MoveToTarget(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	EAnimalState AnimalState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Type")
	EAnimalType AnimalType;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float AcceptanceRadius = 100.0f;

	UPROPERTY()
	AAIController* AnimalController;

	FTimerHandle TargetChangeTimer;
	FTimerHandle EatTimer;
};
