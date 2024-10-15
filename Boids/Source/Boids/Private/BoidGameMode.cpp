// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidGameMode.h"
#include "Kismet/GameplayStatics.h"

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
	ZebraFlocking();
	LionFlocking();
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
	// Print if the mean location has changed
	if (LastZebraMeanLocation != ZebraMeanLocation)
	{
		UE_LOG(LogTemp, Warning, TEXT("Zebra Mean Location: %s"), *ZebraMeanLocation.ToString());
	}
	if (LastCount != AliveZebraCount)
	{
		UE_LOG(LogTemp, Warning, TEXT("Alive Zebra Count: %d"), AliveZebraCount);
	}
}

void ABoidGameMode::LionFlocking()
{
	for (ALion* Lion : Lions)
	{
		if (Lion->GetAnimalState() == EAnimalState::EAS_Hunting)
		{
			if (Lion->HasReachedLocation())
			{
				Lion->LastKnownLocation = ZebraMeanLocation;
			}

			FVector DirectionOfZebras = (Lion->LastKnownLocation - Lion->GetActorLocation()).GetSafeNormal();
			Lion->MoveInDirection(DirectionOfZebras, 0.5f);
		}
		else
		{
			// Wandering phase: If the lion is not hunting, make it wander in a smooth, drifting manner
			if (Lion->GetAnimalState() != EAnimalState::EAS_Hunting)
			{
				Lion->LastKnownLocation = ZebraMeanLocation;
				float DistanceToZebras = FVector::Dist(Lion->GetActorLocation(), Lion->LastKnownLocation);
				if (DistanceToZebras < 1000.0f)
				{
					Lion->BeginHunt();  // Switch to hunting mode
				}
				else
				{
					// Randomly decide whether to apply the drift or not
					if (FMath::RandRange(0.0f, 1.0f) < 0.05f)  // 5% chance to apply the drift
					{
						FVector RandomDrift = FMath::VRand();
						RandomDrift.Z = 0;
						RandomDrift *= 0.01f;

						Lion->CurrentWanderDirection += RandomDrift;

						Lion->CurrentWanderDirection.Normalize();
					}

					Lion->MoveInDirection(Lion->CurrentWanderDirection, 0.15f);
				}
			}
		}
	}
}



