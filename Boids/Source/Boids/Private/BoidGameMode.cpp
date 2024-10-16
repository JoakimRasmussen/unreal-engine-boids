// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidGameMode.h"

ABoidGameMode::ABoidGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoidGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetAllAnimals();
}

void ABoidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//ZebraFlocking();
	UpdateLionTargets();
}

void ABoidGameMode::GetAllAnimals()
{
	Zebras = GetAllZebras();
	Lions = GetAllLions();
}

TArray<AZebra*> ABoidGameMode::GetAllZebras()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAnimal::StaticClass(), ZebraActors);

	TArray<AZebra*> ZebraCollection;
	for (AActor* ZebraActor : ZebraActors)
	{
		AZebra* Zebra = Cast<AZebra>(ZebraActor);
		if (Zebra)
		{
			ZebraCollection.Add(Zebra);
		}
	}
	return ZebraCollection;
}

TArray<ALion*> ABoidGameMode::GetAllLions()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALion::StaticClass(), LionActors);

	TArray<ALion*> LionCollection;
	for (AActor* LionActor : LionActors)
	{
		ALion* Lion = Cast<ALion>(LionActor);
		if (Lion)
		{
			LionCollection.Add(Lion);
		}
	}
	return LionCollection;
}

void ABoidGameMode::ZebraFlocking()
{
	UpdateZebraMeanLocation();

	for (AZebra* Zebra : Zebras)
	{
		if (Zebra->GetAnimalState() == EAnimalState::EAS_Dead) continue;

		FVector ZebraVelocity = Zebra->GetVelocity();
		FVector ZebraLocation = Zebra->GetActorLocation();
		FVector ZebraDirection = ZebraMeanLocation - ZebraLocation;
		ZebraDirection.Normalize();
		Zebra->MoveInDirection(ZebraDirection, 0.5f);
	}
}

void ABoidGameMode::UpdateZebraMeanLocation()
{
	FVector LastZebraMeanLocation = ZebraMeanLocation;
	ZebraMeanLocation = FVector(0.0f, 0.0f, 0.0f);
	int LastCount = AliveZebraCount;
	AliveZebraCount = 0;

	for (AZebra* Zebra : Zebras)
	{
		if (Zebra->GetAnimalState() == EAnimalState::EAS_Dead) continue;

		ZebraMeanLocation += Zebra->GetActorLocation();
		AliveZebraCount++;
	}

	ZebraMeanLocation /= AliveZebraCount;
}

// Function to update each lion's nearest zebra
void ABoidGameMode::UpdateLionTargets()
{
	for (ALion* Lion : Lions)
	{
		if (Lion->GetAnimalState() != EAnimalState::EAS_Dead)
		{
			AZebra* NearestZebra = FindNearestZebra(Lion);
			Lion->SetNearestZebra(NearestZebra);  // Update the lion's nearest zebra
		}
	}
}

AZebra* ABoidGameMode::FindNearestZebra(ALion* Lion)
{
	AZebra* NearestZebra = nullptr;
	float NearestDistance = MAX_FLT;

	for (AZebra* Zebra : Zebras)
	{
		if (Zebra->GetAnimalState() == EAnimalState::EAS_Dead)
		{
			// Skip dead zebras
			continue;
		}

		float DistanceToZebra = FVector::Dist(Lion->GetActorLocation(), Zebra->GetActorLocation());
		if (DistanceToZebra < NearestDistance)
		{
			NearestDistance = DistanceToZebra;
			NearestZebra = Zebra;
		}
	}

	return NearestZebra;
}





