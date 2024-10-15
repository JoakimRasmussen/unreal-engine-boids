// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Lion.generated.h" // This should be the last include

class USphereComponent;

UCLASS()
class BOIDS_API ALion : public AAnimal
{
	GENERATED_BODY()

public:

	ALion();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void BeginHunt();

	UFUNCTION()
	void OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void AttackTarget(AAnimal* Target);
	void EndAttack();
	void CycleStates();

	bool HasReachedLocation();
	bool AttackIsValid();

	FVector LastKnownLocation;
	FVector CurrentWanderDirection;

protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* AttackSphere;

private:

	FTimerHandle AttackEndTimer;
	FTimerHandle StateChangeTimer;
};
