// Fill out your copyright notice in the Description page of Project Settings.


#include "Lion.h"

ALion::ALion()
{
}

void ALion::BeginPlay()
{

	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TargetChangeTimer, this, &ALion::SetRandomTarget, 10.0f, true);
	GetWorldTimerManager().SetTimer(EatTimer, this, &ALion::StartEating, 5.0f, true);
	// Set the initial target
	MoveToTarget(PatrolTarget);
}

void ALion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UE_LOG(LogTemp, Warning, TEXT("Lion is ticking"));
}