// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Zebra.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zebra")
	bool isDead;

	UCharacterMovementComponent* CharacterMovement;
	void SetVelocity(FVector Velocity);
	void SetFleeDirection(FVector CurrentPosition, FVector PredatorPosition);
	void SetMoveDirection(FVector MoveDirection);
	FVector MoveDirection = FVector(1, 0, 0);
	FVector FleeDirection = FVector(0, 0, 0);

private:

	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float CohesionWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AvoidanceWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AlignmentWeight = 0.001f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float AvoidanceRadius = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float PredatorAvoidanceWeight = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float PredatorFleeDistance = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Zebra - Flocking")
	float FlockingRadius = 1000.0f;


public:
	
	FORCEINLINE float GetCohesionWeight() const { return CohesionWeight; }
	FORCEINLINE float GetAvoidanceWeight() const { return AvoidanceWeight; }
	FORCEINLINE float GetAlignmentWeight() const { return AlignmentWeight; }
	FORCEINLINE float GetAvoidanceRadius() const { return AvoidanceRadius; }
	FORCEINLINE float GetPredatorAvoidanceWeight() const { return PredatorAvoidanceWeight; }
	FORCEINLINE float GetPredatorFleeDistance() const { return PredatorFleeDistance; }
	FORCEINLINE float GetFlockingRadius() const { return FlockingRadius; }
	FORCEINLINE FVector GetFleeDirection() const { return FleeDirection; }
	FORCEINLINE float DistanceToActor(AActor* Actor) const { return FVector::Dist(GetActorLocation(), Actor->GetActorLocation()); }

	FORCEINLINE bool IsDead() const { return GetAnimalState() == EAnimalState::EAS_Dead; }
	FORCEINLINE bool IsFleeing() const { return GetAnimalState() == EAnimalState::EAS_Fleeing; }
	
};
