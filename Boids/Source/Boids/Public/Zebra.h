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

	void SetVelocity(FVector Velocity);
	UCharacterMovementComponent* CharacterMovement;

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

public:
	
	FORCEINLINE float GetCohesionWeight() const { return CohesionWeight; }
	FORCEINLINE float GetAvoidanceWeight() const { return AvoidanceWeight; }
	FORCEINLINE float GetAlignmentWeight() const { return AlignmentWeight; }
	FORCEINLINE float GetAvoidanceRadius() const { return AvoidanceRadius; }
	FORCEINLINE float GetPredatorAvoidanceWeight() const { return PredatorAvoidanceWeight; }
	
};
