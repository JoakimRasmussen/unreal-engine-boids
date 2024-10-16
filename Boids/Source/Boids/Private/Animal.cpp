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
	AnimalType = EAnimalType::EAT_Other;
	AnimalState = EAnimalState::EAS_Flocking;
}

// Called when the game starts or when spawned
void AAnimal::BeginPlay()
{
	Super::BeginPlay();

	// Get the AI controller
	AnimalController = Cast<AAIController>(GetController());
}

// Called every frame
void AAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAnimal::Die()
{
	AnimalState = EAnimalState::EAS_Dead;

	if (AnimalController)
	{
		AnimalController->StopMovement();
	}
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

FVector AAnimal::GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius)
{
	// Generate a random angle between 0 and 2*PI (for full circle rotation)
	float RandomAngle = FMath::RandRange(0.0f, 2.0f * PI);

	// Generate a random distance between MinReachRadius and MaxReachRadius
	float RandomRadius = FMath::RandRange(MinReachRadius, MaxReachRadius);

	// Calculate the random point in the XY plane
	float RandomX = RandomRadius * FMath::Cos(RandomAngle);
	float RandomY = RandomRadius * FMath::Sin(RandomAngle);

	// Get the current location of the animal
	FVector CurrentLocation = GetActorLocation();

	// Calculate the random point within the bounds of the two circles
	FVector RandomPoint = FVector(CurrentLocation.X + RandomX, CurrentLocation.Y + RandomY, CurrentLocation.Z);

	// Return the random point
	return RandomPoint;
}

FVector AAnimal::GetRandomPointWithinReach(float ReachRadius)
{
	// Call the overloaded version with MinReachRadius set to 0
	return GetRandomPointWithinReach(0.0f, ReachRadius);
}




// Note: Turn off Flocking when used
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

void AAnimal::MoveTowardsLocation(FVector location)
{
	// Calculate direction vector (and normalize it to make it a unit vector)
	FVector Direction = (location - this->GetActorLocation()).GetSafeNormal();

	// Get current velocity and calculate speed magnitude (how fast the actor is moving)
	FVector ActorSpeed = this->GetVelocity();
	float CurrentSpeed = ActorSpeed.Size();

	// Calculate the relative speed factor (0 to 1) based on current speed and max speed
	float MaxSpeed = this->GetCharacterMovement()->MaxWalkSpeed;
	float RelativeSpeed = CurrentSpeed / MaxSpeed;

	// Move the actor in the given direction, using the calculated relative speed factor
	this->MoveInDirection(Direction, RelativeSpeed);
}

void AAnimal::MoveTowardsLocation(FVector location, float speed)
{
	// Calculate direction vector (and normalize it to make it a unit vector)
	FVector Direction = (location - this->GetActorLocation()).GetSafeNormal();

	// Move the actor in the given direction, using the given speed factor
	this->MoveInDirection(Direction, speed);
}


