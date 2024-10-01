// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "Zebra.generated.h"

/**
 * 
 */
UCLASS()
class BOIDS_API AZebra : public AAnimal
{
	GENERATED_BODY()


public:
	
	AZebra();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

	
};
