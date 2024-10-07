// Fill out your copyright notice in the Description page of Project Settings.


#include "Zebra.h"

AZebra::AZebra()
{
}

void AZebra::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(TargetChangeTimer, this, &AZebra::SetRandomTarget, 6.0f, true);
}

void AZebra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->MoveInDirection(FVector(1.0f, 0.0f, 0.0f), 0.5f );
	UE_LOG(LogTemp, Warning, TEXT("Zebra Velocity: %s"), *GetVelocity().ToString());
}