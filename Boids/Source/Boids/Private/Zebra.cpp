// Fill out your copyright notice in the Description page of Project Settings.


#include "Zebra.h"

AZebra::AZebra()
{
	AnimalType = EAnimalType::EAT_Zebra;
}

void AZebra::BeginPlay()
{
	Super::BeginPlay();
}

void AZebra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->AnimalState == EAnimalState::EAS_Dead)
	{
		return;
	}
	this->MoveInDirection(FVector(1.0f, 0.0f, 0.0f), 0.5f );
}