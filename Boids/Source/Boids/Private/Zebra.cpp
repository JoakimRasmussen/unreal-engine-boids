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
}