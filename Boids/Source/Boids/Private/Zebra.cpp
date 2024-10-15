// Fill out your copyright notice in the Description page of Project Settings.


#include "Zebra.h"

AZebra::AZebra()
{
}

void AZebra::BeginPlay()
{
	Super::BeginPlay();
}

void AZebra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDebugMode)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), GetFlockingRadius(), 12, FColor::Green, false, -1, 0, 1);
	}
	
	if (GetStamina() < 0.0f)
	{
		AnimalState = EAnimalState::EAS_Resting;
		MoveInDirection(FVector(0.0f, 0.0f, 0.0f), 0.0f);
	}
	if (GetAnimalState() == EAnimalState::EAS_Resting)
	{
		if (GetStamina() >= 100.0f)
		{
			AnimalState = EAnimalState::EAS_Flocking;
		}
		if (GetStamina() < 100.0f)
		{
			IncreaseStamina();
		}
	}
}

void AZebra::SetVelocity(FVector Velocity)
{
	CharacterMovement = this->GetCharacterMovement();
	CharacterMovement->Velocity = Velocity;
}
void AZebra::SetFleeDirection(FVector CurrentPosition, FVector PredatorPosition)
{
	// Calculate the direction to flee from the predator
	FleeDirection = CurrentPosition - PredatorPosition;
}
void AZebra::SetMoveDirection(FVector NewDirection)
{
	MoveDirection = NewDirection;
}


void AZebra::AvoidPredator(AActor* Predator, FVector& AvoidanceVector)
{
	float DistanceToPredator = this->DistanceToActor(Predator);
	if (DistanceToPredator < GetAvoidanceRadius())
	{
		if (DistanceToPredator < GetPredatorFleeDistance())
		{
			AnimalState = EAnimalState::EAS_Fleeing;
			SetFleeDirection(GetActorLocation(), Predator->GetActorLocation());
			SetMoveDirection(FleeDirection);
		}
		AvoidanceVector += (GetActorLocation() - Predator->GetActorLocation()) * GetPredatorAvoidanceWeight();
	}
}

void AZebra::FoodSourceAttraction(FVector& AveragePosition)
{
	AveragePosition += GetActorLocation() * GetFoodSourceAttraction();
}

// Name-placeholder... 
void AZebra::FlockingCalculations(AZebra* OtherZebra, FVector& SpeedDifference, FVector& AveragePosition, FVector& AvoidanceVector)
{
	if (OtherZebra->DistanceToActor(this) > this->GetFlockingRadius() || OtherZebra->IsDead() || OtherZebra->IsFleeing())
	{
		return;
	}
	
	if (this != OtherZebra)
	{
		SpeedDifference += OtherZebra->GetVelocity() - this->GetVelocity();
		AveragePosition += OtherZebra->GetActorLocation();
		if (this->DistanceToActor(OtherZebra) < this->GetAvoidanceRadius())
		{
			AvoidanceVector += this->GetActorLocation() - OtherZebra->GetActorLocation();
		}
	}
}

FVector AZebra::CalculateZebraDirection(FVector SpeedDifference, FVector AveragePosition, FVector AvoidanceVector) const
{
	FVector Direction = (this->GetVelocity()
		+ (SpeedDifference * this->GetAlignmentWeight())
		+ (AveragePosition - this->GetActorLocation()) * this->GetCohesionWeight()
		+ AvoidanceVector * this->GetAvoidanceWeight());
		
	return Direction;
}