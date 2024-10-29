// Fill out your copyright notice in the Description page of Project Settings.


#include "Zebra.h"

AZebra::AZebra()
{
	AnimalType = EAnimalType::EAT_Zebra;
	Stamina = MaxStamina;
	Hunger = MaxHunger;
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
	
	OutOfStamina();
	Eating(DeltaTime);
	DrainHunger(DeltaTime);
	Fleeing(DeltaTime);
	Resting(DeltaTime);
}

void AZebra::Eating(float DeltaTime)
{
	if (AnimalState == EAnimalState::EAS_Eating)
	{
		if (Hunger < MaxHunger - 10.0f)
		{
			RegenerateHunger(DeltaTime);
		}
		else
		{
			AnimalState = EAnimalState::EAS_Flocking;
		}
	}
}

void AZebra::FoodWithinReach(FVector FoodSource)
{
	float Distance = FVector::Dist(GetActorLocation(), FoodSource);
	if (Distance < 500.0f)
	{
		AnimalState = EAnimalState::EAS_Eating;
	}
}

void AZebra::OutOfStamina()
{
	if (GetStamina() <= 0.0f)
	{
		AnimalState = EAnimalState::EAS_Resting;
		MoveInDirection(FVector(0.0f, 0.0f, 0.0f), 0.0f);
	}
}

void AZebra::Fleeing(float DeltaTime)
{
	if (GetAnimalState() == EAnimalState::EAS_Fleeing)
	{
		MoveInDirection(GetFleeDirection(), 1.0f);
		DrainStamina(DeltaTime);
	}
}

void AZebra::Resting(float DeltaTime)
{
	if (GetAnimalState() == EAnimalState::EAS_Resting)
	{
		if (GetStamina() >= MaxStamina)
		{
			AnimalState = EAnimalState::EAS_Flocking;
		}
		if (GetStamina() < MaxStamina)
		{
			RegenerateStamina(DeltaTime);
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

void AZebra::FoodSourceAttraction(FVector& AveragePosition, FVector FoodSourceLocation)
{
	AveragePosition += FoodSourceLocation;
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
		AveragePosition += (OtherZebra->GetActorLocation());
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