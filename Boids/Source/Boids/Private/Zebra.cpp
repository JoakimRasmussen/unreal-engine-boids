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
	
	if (GetStamina() < 0.0f)
	{
		AnimalState = EAnimalState::EAS_Resting;
		MoveInDirection(FVector(0.0f, 0.0f, 0.0f), 0.0f);
	}
	
	// if (GetAnimalState() == EAnimalState::EAS_Flocking)
	// {
	// 	MoveInDirection(MoveDirection, 1.0f);
	// }
	
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
