// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Animal.h"
#include "Zebra.h"
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

	void GetAllAnimals();
	
	UPROPERTY(EditAnywhere, Category = "Zebras")
	TArray<AActor*> Zebras;
	
};
