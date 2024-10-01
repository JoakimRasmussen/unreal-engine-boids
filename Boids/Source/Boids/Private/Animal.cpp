// Fill out your copyright notice in the Description page of Project Settings.


#include "Animal.h"
#include "AIController.h"
#include "AITypes.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAnimal::AAnimal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();

	// Initial state
	AnimalState = EAnimalState::EAS_Flocking;
	// Get the AI controller
	AnimalController = Cast<AAIController>(GetController());
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AAnimal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAnimal::MoveToTarget(AActor* Target)
{
	if (AnimalController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	FNavPathSharedPtr NavPath;
	AnimalController->MoveTo(MoveRequest, &NavPath);
}

void AAnimal::StartEating()
{
	AnimalState = EAnimalState::EAS_Eating;
	UE_LOG(LogTemp, Warning, TEXT("Eating"));
}


void AAnimal::SetRandomTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("SetNewTarget"));
	
	if (AnimalController == nullptr) return;

	AnimalState = EAnimalState::EAS_Flocking;

	// Get navigation system
	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSystem == nullptr) return;

	// Get random point on navmesh
	FNavLocation RandomPoint;
	bool bFoundLocation = NavSystem->GetRandomReachablePointInRadius(GetActorLocation(), 1000.0f, RandomPoint);
	
	if (bFoundLocation)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(RandomPoint.Location);
		MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

		FNavPathSharedPtr NavPath;
		AnimalController->MoveTo(MoveRequest, &NavPath);

		UE_LOG(LogTemp, Warning, TEXT("New target set"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No new target set"));
	}
}

