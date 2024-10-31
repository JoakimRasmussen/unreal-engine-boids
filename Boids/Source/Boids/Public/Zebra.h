// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zebra.generated.h" // This should be the last include

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class BOIDS_API AZebra : public AAnimal
{
	GENERATED_BODY()

public:
	
	AZebra();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void OutOfStamina();
	void Eating(float DeltaTime);
	void Fleeing(float DeltaTime);
	void Resting(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zebra")
	bool isDead;

	UCharacterMovementComponent* CharacterMovement;
	FVector MoveDirection = FVector(1, 0, 0);
	FVector FleeDirection = FVector(0, 0, 0);
	
	void SetVelocity(FVector Velocity);
	void SetFleeDirection(FVector CurrentPosition, FVector PredatorPosition);
	void SetMoveDirection(FVector MoveDirection);
	void AvoidPredator(AActor* Predator);
	void AvoidBarrier(AActor* Barrier);
	void FlockingCalculations(AZebra* OtherZebra);
	void FoodSourceAttraction(FVector FoodSourceLocation);
	void FoodWithinReach(FVector FoodSource);
	void CalculateZebraDirection();
	bool IsFarFromHome();

	FVector SpeedDifference;
	FVector AveragePosition;
	FVector AverageVelocity;
	FVector ClosestFoodSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Direction;
	UFUNCTION(BlueprintCallable)
	void ReverseDirection();

private:

	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float CohesionWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AvoidanceWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AlignmentWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float BarrierWeight = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AvoidanceRadius = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float BarrierAvoidanceRadius = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float PredatorAvoidanceWeight = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float PredatorFleeDistance = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float FlockingRadius = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float FoodSourceAttractionWeight = 1.0f;

	FVector HomePosition;

public:
	
	FORCEINLINE float GetCohesionWeight() const { return CohesionWeight; }
	FORCEINLINE float GetAvoidanceWeight() const { return AvoidanceWeight; }
	FORCEINLINE float GetAlignmentWeight() const { return AlignmentWeight; }
	FORCEINLINE float GetAvoidanceRadius() const { return AvoidanceRadius; }
	FORCEINLINE float GetBarrierWeight() const { return BarrierWeight; }
	FORCEINLINE float GetBarrierAvoidanceRadius() const { return BarrierAvoidanceRadius; }
	FORCEINLINE float GetPredatorAvoidanceWeight() const { return PredatorAvoidanceWeight; }
	FORCEINLINE float GetPredatorFleeDistance() const { return PredatorFleeDistance; }
	FORCEINLINE float GetFlockingRadius() const { return FlockingRadius; }
	FORCEINLINE float DistanceToActor(AActor* Actor) const { return FVector::Dist(GetActorLocation(), Actor->GetActorLocation()); }
	FORCEINLINE float GetFoodSourceAttraction() const { return MaxHunger/Hunger; }
	FORCEINLINE float GetHunger() const { return Hunger; }
	
	FORCEINLINE FVector GetHomePosition() const { return HomePosition; }
	FORCEINLINE FVector GetFleeDirection() const { return FleeDirection; }

	FORCEINLINE bool IsDead() const { return GetAnimalState() == EAnimalState::EAS_Dead; }
	FORCEINLINE bool IsFleeing() const { return GetAnimalState() == EAnimalState::EAS_Fleeing; }
	
};
