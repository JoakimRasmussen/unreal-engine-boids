// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "Zebra.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
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
	void TransitionToHunting();
	void EndAttack();
	bool AttackIsValid();
	bool ShouldExitResting();
	bool NeedRest();
	bool EnoughStamina(); // Enough stamina to sprint
	bool IsHungry(); // Hungry enough to hunt
	void TransitionToWandering();
	void TransitionToResting();

	// Behavior and action functions
	void AttackTarget(AAnimal* Target);
	void SetWanderDirection();
	bool ZebraInSight();

	// Utility functions
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

	// AI-related combat variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Combat")
	float AttackCooldown = 2.0f;

	float SightRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Combat")
	float DefaultSightRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Combat")
	float DesperateSightRadius = 2000.0f;

	bool bIsAttacking = false;
	bool bIsDesperate = false;

	// AI-related speed variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Speed", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxSprintSpeed = 0.80f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Speed", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinWanderSpeed = 0.10f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Speed", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxWanderSpeed = 0.30f;

	// AI-related stamina variables
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StaminaHuntThreshold = 0.40f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float StaminaRestThreshold = 0.30f;

	float StaminaDrainRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float DefaultStaminaDrainRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float DesperateStaminaDrainRate = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float WanderingStaminaDrainMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float SprintingStaminaDrainMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Stamina")
	float StaminaRegenRate = 4.0f;


	// AI-related hunger variables
	float Hunger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger")
	float MaxHunger = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HungerThreshold = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DesperateHungerThreshold = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger")
	float HungerDrainRate = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger")
	float RestingHungerDrainMultiplier = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI - Hunger")
	float HuntingHungerDrainMultiplier = 2.0f;

	void DrainStamina(float DeltaTime);
	void RegenerateStamina(float DeltaTime);
	void DrainHunger(float DeltaTime);
	bool HasStarved();
	bool IsDesperate();
	float CalculateSpeedFromStamina();
	void MoveTowardsZebra();

	// Timer handles
	FTimerHandle AttackEndTimer;

	// Nearest zebra reference
	AZebra* NearestZebra;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DebugWidgetClass;

	UUserWidget* DebugWidgetInstance;
	void UpdateDebugWidget();

};

