// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "Zebra.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Lion.generated.h" // This should be the last include

// Forward declaration
class USphereComponent;

UCLASS()
class BOIDS_API ALion : public AAnimal
{
	GENERATED_BODY()

public:
	// Constructor
	ALion();

	// Lifecycle functions
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// State transition functions
	void BeginHunt();
	void EndAttack();
	bool AttackIsValid();
	bool ShouldExitResting();
	void TransitionToWandering();

	// Behavior and action functions
	void AttackTarget(AAnimal* Target);
	void SetWanderDirection();
	bool IsZebraInSight();

	// Utility functions
	FVector ChooseRandomPointWithinReach(float ReachRadius);
	bool HasReachedLocation();

	// Collision handling
	UFUNCTION()
	void OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Setter for nearest zebra
	void SetNearestZebra(AZebra* Zebra) { NearestZebra = Zebra; }

protected:
	// Components
	UPROPERTY(VisibleAnywhere)
	USphereComponent* AttackSphere;

	// Movement variables
	FVector CurrentWanderPoint;
	FVector CurrentWanderDirection;

	// AI-related variables
	float AttackCooldown = 2.0f;
	float SightRadius = 1000.0f;

	// Timer handles
	FTimerHandle AttackEndTimer;

	// Nearest zebra reference
	AZebra* NearestZebra;
};
