// Fill out your copyright notice in the Description page of Project Settings.

#include "Lion.h"

// Constructor
ALion::ALion()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set the capsule component as the root component
    RootComponent = GetCapsuleComponent();

    // Initialize the attack sphere and attach it to the root component
    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->InitSphereRadius(250.0f);
    AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ALion::OnAttackSphereOverlap);

    // Set default animal type and initial state
    AnimalType = EAnimalType::EAT_Lion;
    AnimalState = EAnimalState::EAS_Resting;
}

// Lifecycle Functions
void ALion::BeginPlay()
{
    Super::BeginPlay();
}

void ALion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetAnimalState() == EAnimalState::EAS_Dead)
    {
        return; // Skip processing if lion is dead
    }

    switch (GetAnimalState())
    {
    case EAnimalState::EAS_Resting:
        if (ShouldExitResting())
        {
            TransitionToWandering();
        }
        break;

    case EAnimalState::EAS_Wandering:
        if (HasReachedLocation())
        {
            CurrentWanderPoint = ChooseRandomPointWithinReach(1000.0f);
        }
        SetWanderDirection();
        MoveInDirection(CurrentWanderDirection, 0.25f);

        if (IsZebraInSight())
        {
            BeginHunt();
        }
        break;

    case EAnimalState::EAS_Hunting:
        if (NearestZebra)
        {
            FVector ZebraLocation = NearestZebra->GetActorLocation();
            FVector DirectionToZebra = ZebraLocation - GetActorLocation();
            DirectionToZebra.Normalize();
            MoveInDirection(DirectionToZebra, 0.8f);
        }
        else
        {
            TransitionToWandering();
        }
        break;

    case EAnimalState::EAS_Attacking:
        // Attack state logic is handled in AttackTarget or EndAttack
        break;

    default:
        break;
    }
}

// State Transition Functions
void ALion::BeginHunt()
{
    UE_LOG(LogTemp, Warning, TEXT("Lion has started hunting!"));
    AnimalState = EAnimalState::EAS_Hunting;
}

void ALion::EndAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("Lion has finished attacking!"));
    GetCharacterMovement()->StopMovementImmediately();  // Stop movement after attack
    AnimalState = EAnimalState::EAS_Wandering;
    NearestZebra = nullptr; // Reset the nearest zebra
    GetWorldTimerManager().ClearTimer(AttackEndTimer);
}

bool ALion::AttackIsValid()
{
    return AnimalState == EAnimalState::EAS_Hunting; // Must be in hunting state to attack
}

bool ALion::ShouldExitResting()
{
    return true; // Exits after some time, can be expanded with additional logic
}

void ALion::TransitionToWandering()
{
    AnimalState = EAnimalState::EAS_Wandering;
    CurrentWanderPoint = ChooseRandomPointWithinReach(1000.0f);
}

// Movement and Direction Functions
FVector ALion::ChooseRandomPointWithinReach(float ReachRadius)
{
    FVector CurrentLocation = GetActorLocation();
    FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(CurrentLocation, FVector(ReachRadius, ReachRadius, 0.0f));
    RandomPoint.Z = CurrentLocation.Z; // Ensure movement is only in the X/Y plane
    return RandomPoint;
}

void ALion::SetWanderDirection()
{
    CurrentWanderDirection = CurrentWanderPoint - GetActorLocation();
    CurrentWanderDirection.Normalize();
}

bool ALion::HasReachedLocation()
{
    return FVector::Dist(GetActorLocation(), CurrentWanderPoint) < 100.0f; // Example threshold
}

bool ALion::IsZebraInSight()
{
    if (!NearestZebra) return false;
    float DistanceToZebra = FVector::Dist(GetActorLocation(), NearestZebra->GetActorLocation());
    return DistanceToZebra < SightRadius;
}

// Attack Handling
void ALion::AttackTarget(AAnimal* Target)
{
    UE_LOG(LogTemp, Warning, TEXT("Lion has started attacking!"));
    AnimalState = EAnimalState::EAS_Attacking;

    if (Target)
    {
        FVector LionLocation = GetActorLocation();
        FVector TargetLocation = Target->GetActorLocation();
        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(LionLocation, TargetLocation);
        SetActorRotation(LookAtRotation);
    }

    // Set a timer to end the attack after AttackCooldown seconds
    GetWorldTimerManager().SetTimer(AttackEndTimer, this, &ALion::EndAttack, AttackCooldown, false);
}

// Collision Handling
void ALion::OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {
        AAnimal* Animal = Cast<AAnimal>(OtherActor);
        if (Animal)
        {
            if (Animal->GetAnimalType() == EAnimalType::EAT_Zebra)
            {
                if (!AttackIsValid())
                {
                    UE_LOG(LogTemp, Warning, TEXT("Lion is not in the correct state to attack."));
                    return;
                }
                AttackTarget(Animal);
                Animal->OnDeath();
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("The overlapped actor is not of type AAnimal."));
        }
    }
}
