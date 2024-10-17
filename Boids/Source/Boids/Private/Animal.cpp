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

float AAnimal::CalculateSpeedFromStamina()
{
	float NormalizedStamina = Stamina / MaxStamina;
	float LowerBound = MinWalkingSpeed; // Could change depending on specific states
	float UpperBound = MaxSprintSpeed;  // Could change depending on specific states

	float SpeedFactor = FMath::Cos(NormalizedStamina * PI / 2 - PI / 2); // Cosine interpolation
	return FMath::Lerp(LowerBound, UpperBound, SpeedFactor);
}

void AAnimal::DrainStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina - StaminaDrainRate * DeltaTime, 0.0f, MaxStamina);
}

void AAnimal::RegenerateStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
}

void AAnimal::DrainHunger(float DeltaTime)
{
	float CurrentDrainRate = (AnimalState == EAnimalState::EAS_Resting) ? 0.5 * HungerDrainRate : HungerDrainRate;
	Hunger = FMath::Clamp(Hunger - CurrentDrainRate * DeltaTime, 0.0f, MaxHunger);
}

bool AAnimal::NeedRest()
{
	return Stamina <= StaminaRestThreshold * MaxStamina;
}

bool AAnimal::ShouldExitResting()
{
	return Stamina >= MaxStamina;
}

bool AAnimal::EnoughStamina()
{
	return Stamina >= 0.4 * MaxStamina;
}

bool AAnimal::IsHungry()
{
	return Hunger <= HungerThreshold * MaxHunger;
}

bool AAnimal::HasStarved()
{
	return Hunger <= 0.0f;
}

bool AAnimal::HasReachedLocation(FVector Location)
{
	return FVector::Dist(GetActorLocation(), Location) <= AcceptanceRadius;
}

void AAnimal::StartEating()
{
	AnimalState = EAnimalState::EAS_Eating;
}

void AAnimal::StartWandering()
{
	AnimalState = EAnimalState::EAS_Wandering;
}

void AAnimal::StartResting()
{
	AnimalState = EAnimalState::EAS_Resting;
}

FVector AAnimal::GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius, float ConeAngleDegrees)
{
	FVector ForwardDirection = GetActorForwardVector();

	float HalfConeAngle = ConeAngleDegrees / 2.0f;
	float RandomAngle = FMath::RandRange(-HalfConeAngle, HalfConeAngle);

	float RandomRadius = FMath::RandRange(MinReachRadius, MaxReachRadius);

	// Create a rotation matrix from the random angle to rotate the forward direction
	FRotator Rotation(0.0f, RandomAngle, 0.0f);
	FVector RandomDirection = Rotation.RotateVector(ForwardDirection);

	// Scale the random direction by the random radius to get a random point within the cone
	FVector RandomPoint = GetActorLocation() + (RandomDirection * RandomRadius);

	return RandomPoint;
}

FVector AAnimal::GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius)
{
	return GetRandomPointWithinReach(MinReachRadius, MaxReachRadius, 360.0f);
}

FVector AAnimal::GetRandomPointWithinReach(float ReachRadius)
{
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

void AAnimal::MoveTowardsLocation(FVector location, float speedFactor)
{
	// Calculate direction vector (and normalize it to make it a unit vector)
	FVector Direction = (location - this->GetActorLocation()).GetSafeNormal();

	// Move the actor in the given direction, using the given speed factor
	this->MoveInDirection(Direction, speedFactor);
}

void AAnimal::MoveTowardsOtherAnimal(AAnimal* OtherAnimal, float speedFactor)
{
	if (OtherAnimal == nullptr) return;
	FVector OtherAnimalLocation = OtherAnimal->GetActorLocation();
	this->MoveTowardsLocation(OtherAnimalLocation, speedFactor);
}


