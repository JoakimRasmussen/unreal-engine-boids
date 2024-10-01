// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidGameMode.h"
#include "Kismet/GameplayStatics.h"

ABoidGameMode::ABoidGameMode()
{
}

void ABoidGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetAllAnimals();
}

void ABoidGameMode::GetAllAnimals()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimal::StaticClass(), Zebras);
	
	for (AActor* Zebra : Zebras)
	{
		UE_LOG(LogTemp, Warning, TEXT("Zebra: %s"), *Zebra->GetName());
	}
}
