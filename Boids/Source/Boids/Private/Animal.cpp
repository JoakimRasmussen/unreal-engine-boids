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

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);  // Allow overlap with lion’s AttackSphere
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);  // Ensure overlaps are enabled

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

void AAnimal::SetDeadState()
{
	AnimalState = EAnimalState::EAS_Dead;
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

void AAnimal::RegenerateHunger(float DeltaTime)
{
	Hunger = FMath::Clamp(Hunger + HungerRegenRate * DeltaTime, 0.0f, MaxHunger);
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

bool AAnimal::IsStuck()
{
	if (AnimalState == EAnimalState::EAS_Resting) return false;
	if (Speed <= 0.1f) return true; // Fast check for stuck state when not moving

	// Get the current location and time
	FVector CurrentLocation = GetActorLocation();
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Check if enough time has passed since the last check
	if (CurrentTime - LastCheckTime >= CheckInterval)
	{
		// Calculate the horizontal distance moved
		FVector FlatCurrentLocation = CurrentLocation;
		FlatCurrentLocation.Z = 0.0f;
		FVector FlatLastCheckedLocation = LastCheckedLocation;
		FlatLastCheckedLocation.Z = 0.0f;
		float DistanceMoved = FVector::Dist(FlatCurrentLocation, FlatLastCheckedLocation);

		// Update last check time and position
		LastCheckedLocation = FlatCurrentLocation;
		LastCheckTime = CurrentTime;

		// If the distance moved is below the threshold, consider it stuck
		return DistanceMoved < StuckThresholdDistance;
	}

	// If not enough time has passed, assume not stuck
	return false;
}


bool AAnimal::HasReachedLocation(FVector Location) // Ignore Z
{
	FVector CurrentLocation = GetActorLocation();
	CurrentLocation.Z = 0.0f;
	FVector TargetLocation = Location;
	TargetLocation.Z = 0.0f;

	float Distance = FVector::Dist(CurrentLocation, TargetLocation);
	return Distance <= 10.0f;
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
	return GetRandomPointWithinReach(MinReachRadius, MaxReachRadius, ConeAngleDegrees, false);
}

FVector AAnimal::GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius, float ConeAngleDegrees, bool bBehind)
{
	// Determine the forward direction, and invert it if the cone should be behind the animal
	FVector ForwardDirection = bBehind ? -GetActorForwardVector() : GetActorForwardVector();

	float HalfConeAngle = ConeAngleDegrees / 2.0f;
	float RandomAngle = FMath::RandRange(-HalfConeAngle, HalfConeAngle);

	float RandomRadius = FMath::RandRange(MinReachRadius, MaxReachRadius);

	// Create a rotation matrix from the random angle to rotate the forward (or backward) direction
	FRotator Rotation(0.0f, RandomAngle, 0.0f);
	FVector RandomDirection = Rotation.RotateVector(ForwardDirection);

	// Scale the random direction by the random radius to get a random point within the cone
	FVector RandomPoint = GetActorLocation() + (RandomDirection * RandomRadius);

	return RandomPoint;
}

FVector AAnimal::GetRandomPointWithinReach(float MinReachRadius, float MaxReachRadius)
{
	return GetRandomPointWithinReach(MinReachRadius, MaxReachRadius, 360.0f, false);
}

FVector AAnimal::GetRandomPointWithinReach(float ReachRadius)
{
	return GetRandomPointWithinReach(0.0f, ReachRadius, 360.0f, false);
}

FVector AAnimal::GetRandomPointNear(FVector TargetLocation, float MinReachRadius, float MaxReachRadius)
{
	float RandomRadius = FMath::RandRange(MinReachRadius, MaxReachRadius);
	FVector RandomPoint = TargetLocation + (FMath::VRand() * RandomRadius);
	RandomPoint.Z = GetActorLocation().Z; // Keep the Z coordinate the same as the animal's location

	return RandomPoint;
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

void AAnimal::MoveTowardsLocation(FVector DesiredLocation, float SpeedFactor)
{
	// Calculate direction vector (and normalize it to make it a unit vector)
	FVector Direction = (DesiredLocation - this->GetActorLocation()).GetSafeNormal();

	// Move the actor in the given direction, using the given speed factor
	this->MoveInDirection(Direction, SpeedFactor);
}

void AAnimal::MoveTowardsOtherAnimal(AAnimal* OtherAnimal, float SpeedFactor)
{
	if (OtherAnimal == nullptr) return;
	FVector OtherAnimalLocation = OtherAnimal->GetActorLocation();
	this->MoveTowardsLocation(OtherAnimalLocation, SpeedFactor);
}

bool AAnimal::IsPathBlocked(FVector TargetLocation)
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = TargetLocation;

	// Line trace (raycast) to check if there's an obstacle in the way
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);  // Ignore the animal itself

	// Perform the line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,  // Use the visibility channel, adjust if needed
		CollisionParams
	);

	// Debug line for visualization (optional)
	if (bDebugMode)
	{
		FColor LineColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugLine(GetWorld(), Start, End, LineColor, false, 1.0f, 0, 2.0f);
	}

	// Return true if the line trace hit an obstacle, indicating the path is blocked
	return bHit;
}


