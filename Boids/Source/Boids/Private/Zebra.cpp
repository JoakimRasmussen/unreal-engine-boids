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
	if (GetAnimalState() != EAnimalState::EAS_Dead)
	{
		MoveInDirection(FVector(1, 0, 0), 0.5f);
	}
	
}

void AZebra::SetVelocity(FVector Velocity)
{
	CharacterMovement = this->GetCharacterMovement();
	CharacterMovement->Velocity = Velocity;
}
