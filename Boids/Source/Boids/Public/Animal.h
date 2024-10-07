// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalStates.h"
#include "Animal.generated.h"

class AAIController;

UCLASS()
class BOIDS_API AAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAnimal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Set the state of the animal, used in animation blueprint to controll state machine
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "AI Navigation")
	EAnimalState AnimalState;

private:
	

protected:
	
	/*** Everything used by all ANIMALS should be collected here ***/

	// Sets a random position to walk towards
	void SetRandomTarget();
	void StartEating();
	// Set a specific target, could be put to spawn point (home, nest, etc.)
	void MoveToTarget(AActor* Target);
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	AActor* PatrolTarget;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float AcceptanceRadius = 100.0f;
	
	UPROPERTY()
	class AAIController* AnimalController;
	FTimerHandle TargetChangeTimer;
	FTimerHandle EatTimer;

public:
	
	UFUNCTION()
	FORCEINLINE EAnimalState GetAnimalState() const { return AnimalState; }
	FORCEINLINE AAIController* GetAnimalController() const { return AnimalController; }
	FORCEINLINE FVector GetAnimalPosition() const { return GetActorLocation(); }
	FORCEINLINE FVector GetAnimalVelocity() const { return GetCharacterMovement()->Velocity; }

};
