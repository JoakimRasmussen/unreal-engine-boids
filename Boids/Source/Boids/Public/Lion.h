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
	void StartHunting();
	void EndAttack();
	bool AttackIsValid();
	void StartWandering() override;

	void HandleRestingState(float DeltaTime);
	void HandleWanderingState(float DeltaTime);
	void HandleHuntingState(float DeltaTime);
	void HandleAttackingState(float DeltaTime);
	void HandleDesperation();

	bool ShouldExitResting() override;
	bool EnoughStamina() override;
	bool IsDesperate();

	float CalculateSpeedFromStamina() override;

	void DrainStamina(float DeltaTime) override;
	void RegenerateStamina(float DeltaTime) override;
	void DrainHunger(float DeltaTime) override;

	// Behavior and action functions
	void AttackTarget(AAnimal* Target);
	bool ZebraInSight();

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

	bool bIsAttacking = false;

	// AI-related combat variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Combat", meta = (ToolTip = "The cooldown time (in seconds) between attacks"))
	float AttackCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Vision", meta = (ToolTip = "The lion's default sight radius for detecting zebras"))
	float DefaultSightRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Vision", meta = (ToolTip = "The lion's extended sight radius when desperate for food"))
	float DesperateSightRadius = 2000.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "Stamina threshold for initiating a hunt"))
	float StaminaHuntThreshold = 0.40f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (ToolTip = "Stamina drain rate when the lion is desperate"))
	float DesperateStaminaDrainRate = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (ToolTip = "Stamina drain multiplier when wandering"))
	float WanderingStaminaDrainMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stamina", meta = (ToolTip = "Stamina drain multiplier when sprinting"))
	float SprintingStaminaDrainMultiplier = 2.0f;

	float DefaultStaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger", meta = (ClampMin = "0.0", ClampMax = "1.0", ToolTip = "The percentage of MaxHunger below which the lion becomes desperate for food (e.g., 0.20 means desperation starts when hunger is below 20%)."))
	float DesperateHungerThreshold = 0.20f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger", meta = (ToolTip = "Hunger drain multiplier while resting"))
	float RestingHungerDrainMultiplier = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Hunger", meta = (ToolTip = "Hunger drain multiplier while hunting"))
	float HuntingHungerDrainMultiplier = 2.0f;



	// Timer handles
	FTimerHandle AttackEndTimer;

	// Nearest zebra reference
	AZebra* NearestZebra;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> DebugWidgetClass;

	UUserWidget* DebugWidgetInstance;
	void UpdateDebugWidget();

};

