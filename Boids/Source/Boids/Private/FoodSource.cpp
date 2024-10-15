// Fill out your copyright notice in the Description page of Project Settings.


#include "FoodSource.h"

// Sets default values
AFoodSource::AFoodSource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FoodMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FoodMesh"));
	FoodMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AFoodSource::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFoodSource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

