// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animal.h"
#include "Zebra.h"
#include "Lion.h"
#include "BoidGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BOIDS_API ABoidGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ABoidGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void GetAllAnimals();
	void ZebraFlocking();
	void LionFlocking();
	void UpdateLionTargets();

private:
	TArray<AActor*> LionActors;
	TArray<ALion*> Lions;

	TArray<AActor*> ZebraActors;
	TArray<AZebra*> Zebras;
	FVector ZebraMeanLocation;
	int AliveZebraCount;

	TArray<AZebra*> GetAllZebras();
	TArray<ALion*> GetAllLions();

	void UpdateZebraMeanLocation();
	AZebra* FindNearestZebra(ALion* Lion);
};
