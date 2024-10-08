// Fill out your copyright notice in the Description page of Project Settings.


#include "Lion.h"

ALion::ALion()
{
}

void ALion::BeginPlay()
{
	Super::BeginPlay();
}

void ALion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALion::BeginHunt()
{
	this->isHunting = true;

	UE_LOG(LogTemp, Warning, TEXT("Lion has started hunting!"));
}

bool ALion::hasReachedLocation()
{
	return FVector::Dist(this->GetActorLocation(), this->LastKnownLocation) < 100.0f;
}