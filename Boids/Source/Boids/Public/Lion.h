// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animal.h"
#include "Lion.generated.h"

/**
 * 
 */
UCLASS()
class BOIDS_API ALion : public AAnimal
{
	GENERATED_BODY()

public:

	ALion();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

};
