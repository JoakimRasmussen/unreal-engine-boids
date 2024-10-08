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

	if (this->isDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("Zebra is dead"));
		return;
	}
	this->MoveInDirection(FVector(1.0f, 0.0f, 0.0f), 0.5f );
	UE_LOG(LogTemp, Warning, TEXT("Zebra Velocity: %s"), *GetVelocity().ToString());
}