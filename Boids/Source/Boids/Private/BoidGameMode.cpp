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
	FoodSources = GetAllFoodSources();
}

void ABoidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZebraFlocking();
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

TArray<AFoodSource*> ABoidGameMode::GetAllFoodSources()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFoodSource::StaticClass(), FoodSourceActors);

	TArray<AFoodSource*> FoodSourceCollection;
	for (AActor* FoodSourceActor : FoodSourceActors)
	{
		if (FoodSourceActor != nullptr)
		{
			AFoodSource* FoodSource = Cast<AFoodSource>(FoodSourceActor);
			if (FoodSource)
			{
				FoodSourceCollection.Add(FoodSource);
			}
		}
	}
	return FoodSourceCollection;
}

void ABoidGameMode::ZebraFlocking()
{
	UpdateZebraMeanLocation();

	FVector SpeedDifference;
	FVector AveragePosition;
	FVector AvoidanceVector;
	FVector ClosestFoodSourcePosition;
	FVector Direction;
	
	float DistanceToFoodSource;
	float DistanceToClosestFoodSource;
	int Count;

	for (AZebra* Zebra : Zebras)
	{
		if (Zebra->IsDead())
		{
			continue;
		}

		SpeedDifference = FVector(0, 0, 0);
		AveragePosition = Zebra->GetActorLocation();
		AvoidanceVector = FVector(0, 0, 0);
		ClosestFoodSourcePosition = FVector(0, 0, 0);
		DistanceToClosestFoodSource = INFINITY;
		
		// Regular flocking
		if (Zebra->GetAnimalState() == EAnimalState::EAS_Flocking)
		{
			Count = 0;
			
			// 6-th sense to avoid Lions. (Scent perhaps)
			for (ALion* Lion : Lions)
			{
				Zebra->AvoidPredator(Lion, AvoidanceVector);
			}


			for (AZebra* OtherZebra : Zebras)
			{
				if (Zebra->DistanceToActor(OtherZebra) < Zebra->GetFlockingRadius())
				{
					Zebra->FlockingCalculations(OtherZebra, SpeedDifference, AveragePosition, AvoidanceVector);
					Count++;
				}
			}
			
			SpeedDifference /= Count;
			AveragePosition /= Count;
			AvoidanceVector /= Count;

			if (FoodSources.Num() != 0)
			{
				for (AFoodSource* FoodSource : FoodSources)
				{
					DistanceToFoodSource = Zebra->DistanceToActor(FoodSource);
					if (DistanceToFoodSource < DistanceToClosestFoodSource)
					{
						DistanceToClosestFoodSource = DistanceToFoodSource;
						ClosestFoodSourcePosition = FoodSource->GetActorLocation();
					}
				}
			}
			
			Direction = Zebra->CalculateZebraDirection(SpeedDifference, AveragePosition, AvoidanceVector);
			if (Zebra->GetHunger() < 10.0f)
			{
				// Need to find a smart way to adjust SpeedFactor...
				Zebra->MoveInDirection((ClosestFoodSourcePosition - Zebra->GetActorLocation()).GetSafeNormal(), 0.5f);
				Zebra->FoodWithinReach(ClosestFoodSourcePosition);
			}
			else
			{
				// Need to find a smart way to adjust SpeedFactor...
				Zebra->MoveInDirection(Direction.GetSafeNormal(), 0.5f);
			}
		}
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
		// UE_LOG(LogTemp, Warning, TEXT("Zebra Mean Location: %s"), *ZebraMeanLocation.ToString());
	}
	if (LastCount != AliveZebraCount)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Alive Zebra Count: %d"), AliveZebraCount);
	}
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


