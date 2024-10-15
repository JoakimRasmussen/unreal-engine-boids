// Fill out your copyright notice in the Description page of Project Settings.


#include "Lion.h"

ALion::ALion()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set the capsule component as the root component
    RootComponent = GetCapsuleComponent();

    // Initialize the attack sphere and attach it to the root component
    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->InitSphereRadius(200.0f);
    AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ALion::OnAttackSphereOverlap);

    // Set the lion's default animal type and state
    AnimalType = EAnimalType::EAT_Lion;
	AnimalState = EAnimalState::EAS_Resting;
}

void ALion::BeginPlay()
{
	Super::BeginPlay();

    // Start a timer to transition states every 5 seconds (adjust the time as needed)
    //GetWorldTimerManager().SetTimer(StateChangeTimer, this, &ALion::CycleStates, 5.0f, true);  // 5-second interval
}

void ALion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FVector ForwardVector = GetActorForwardVector();
	if (AnimalState == EAnimalState::EAS_Wandering)
	{
		MoveInDirection(ForwardVector, 0.5f);
	}
    else if (AnimalState == EAnimalState::EAS_Hunting)
    {
		MoveInDirection(ForwardVector, 0.8f);
    }

}

// Only for Development purposes
void ALion::CycleStates()
{
    switch (AnimalState)
    {
    case EAnimalState::EAS_Resting:
        UE_LOG(LogTemp, Warning, TEXT("Lion is transitioning from Resting to Wandering!"));
        AnimalState = EAnimalState::EAS_Wandering;
        break;

    case EAnimalState::EAS_Wandering:
        UE_LOG(LogTemp, Warning, TEXT("Lion is transitioning from Wandering to Hunting!"));
        AnimalState = EAnimalState::EAS_Hunting;
        break;

	case EAnimalState::EAS_Hunting:
		UE_LOG(LogTemp, Warning, TEXT("Lion is transitioning from Hunting to Attacking!"));
		AnimalState = EAnimalState::EAS_Attacking;
		break;
    
	case EAnimalState::EAS_Attacking:
		UE_LOG(LogTemp, Warning, TEXT("Lion is transitioning from Attacking to Dead!"));
		AnimalState = EAnimalState::EAS_Dead;
		break;

    default:
        break;
    }
}

void ALion::BeginHunt()
{
	UE_LOG(LogTemp, Warning, TEXT("Lion has started hunting!"));

	AnimalState = EAnimalState::EAS_Hunting;
}

bool ALion::HasReachedLocation()
{
	return FVector::Dist(this->GetActorLocation(), this->LastKnownLocation) < 100.0f;
}

void ALion::OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Lion has overlapped with something!"));

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

    // Set a timer to end the attack after X seconds
    GetWorldTimerManager().SetTimer(AttackEndTimer, this, &ALion::EndAttack, 2.0f, false);
}

void ALion::EndAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("Lion has finished attacking!"));

    AnimalState = EAnimalState::EAS_Wandering;

    GetWorldTimerManager().ClearTimer(AttackEndTimer);
}

// Must be in Hunting state to be able to attack
bool ALion::AttackIsValid()
{
	return AnimalState == EAnimalState::EAS_Hunting;
}
