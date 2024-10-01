// Fill out your copyright notice in the Description page of Project Settings.


#include "Zebra.h"

AZebra::AZebra()
{
}

void AZebra::BeginPlay()
{

	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(TargetChangeTimer, this, &AZebra::SetRandomTarget, 10.0f, true);
	GetWorldTimerManager().SetTimer(EatTimer, this, &AZebra::StartEating, 5.0f, true);
	// Set the initial target
	MoveToTarget(PatrolTarget);
}

void AZebra::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
