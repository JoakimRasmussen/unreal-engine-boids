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

	Stamina = MaxStamina; // Initialize stamina
	Hunger = MaxHunger; // Initialize hunger

    // Check if the widget class is assigned in the editor
    if (DebugWidgetClass)
    {
        DebugWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DebugWidgetClass);
        if (DebugWidgetInstance)
        {
            DebugWidgetInstance->AddToViewport(); // Add the widget to the screen
        }
    }
}

void ALion::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetAnimalState() == EAnimalState::EAS_Dead)
    {
        return; // Skip processing if lion is dead
    }

	// Drain hunger over time and check if the lion has starved
	DrainHunger(DeltaTime);
	if (HasStarved()) { this->OnDeath(); }



    switch (GetAnimalState())
    {
    case EAnimalState::EAS_Resting:
		RegenerateStamina(DeltaTime);
        if (ShouldExitResting())
        {
            TransitionToWandering();
        }
        break;

    case EAnimalState::EAS_Wandering:
		DrainStamina(DeltaTime);
		if (Stamina <= 0.2*MaxStamina)
		{
			TransitionToResting();
		}
        if (HasReachedLocation())
        {
            CurrentWanderPoint = ChooseRandomPointWithinReach(1000.0f);
        }
        SetWanderDirection();
        MoveInDirection(CurrentWanderDirection, 0.25f);

        if (IsZebraInSight() && Stamina > MinStaminaThreshold && Hunger < 0.8f * MaxHunger )
        {
            BeginHunt();
        }
        break;

    case EAnimalState::EAS_Hunting:
		DrainStamina(DeltaTime, 2.0f);

		// Stamina is less than threshold or zebra is out of sight
		if (Stamina <= MinStaminaThreshold || !IsZebraInSight())
		{
			TransitionToWandering();
		}

		else if (NearestZebra)
		{
			FVector ZebraLocation = NearestZebra->GetActorLocation();
			FVector DirectionToZebra = ZebraLocation - GetActorLocation();
			DirectionToZebra.Normalize();
			MoveInDirection(DirectionToZebra, CalculateSpeedFromStamina());
		}

		break;

    case EAnimalState::EAS_Attacking:
        // Attack state logic is handled in AttackTarget or EndAttack
        break;

    default:
        break;
    }

    UpdateDebugWidget();
}

// State Transition Functions
void ALion::BeginHunt()
{
    AnimalState = EAnimalState::EAS_Hunting;
}

void ALion::EndAttack()
{
    GetCharacterMovement()->StopMovementImmediately();  // Stop movement after attack
    AnimalState = EAnimalState::EAS_Wandering;
    NearestZebra = nullptr; // Reset the nearest zebra
    GetWorldTimerManager().ClearTimer(AttackEndTimer);
}

// Must be in hunting state and hungry to attack
bool ALion::AttackIsValid()
{
	return AnimalState == EAnimalState::EAS_Hunting && Hunger < 0.8f*MaxHunger;
}

bool ALion::ShouldExitResting()
{
	return Stamina >= MaxStamina;
}

void ALion::TransitionToWandering()
{
    AnimalState = EAnimalState::EAS_Wandering;
    CurrentWanderPoint = ChooseRandomPointWithinReach(1000.0f);
}

void ALion::TransitionToResting()
{
	AnimalState = EAnimalState::EAS_Resting;
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
                if (!AttackIsValid() && Animal->GetAnimalState() == EAnimalState::EAS_Dead)
                {
                    return;
                }
                AttackTarget(Animal);
                Animal->OnDeath();

				Hunger = MaxHunger; // Reset hunger after eating
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("The overlapped actor is not of type AAnimal."));
        }
    }
}

void ALion::DrainStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina - StaminaDrainRate * DeltaTime, 0.0f, MaxStamina);
}

void ALion::DrainStamina(float DeltaTime, float DrainMultiplier)
{
	Stamina = FMath::Clamp(Stamina - StaminaDrainRate * DeltaTime * DrainMultiplier, 0.0f, MaxStamina);
}

void ALion::RegenerateStamina(float DeltaTime)
{
	float RegenRate = StaminaRegenRate;
    if (Hunger < MaxHunger / 2)
    {
        RegenRate *= 0.8f;  // Slow regeneration if hunger is low
    }

	Stamina = FMath::Clamp(Stamina + RegenRate * DeltaTime, 0.0f, MaxStamina);
}

void ALion::DrainHunger(float DeltaTime)
{
	if (AnimalState == EAnimalState::EAS_Resting)
	{
		Hunger = FMath::Clamp(Hunger - HungerDrainRate * DeltaTime * 0.5f, 0.0f, MaxHunger);
	}
	else
	{
		Hunger = FMath::Clamp(Hunger - HungerDrainRate * DeltaTime, 0.0f, MaxHunger);
	}
}

bool ALion::HasStarved()
{
	return Hunger <= 0.0f;
}

float ALion::CalculateSpeedFromStamina()
{
	float NormalizedStamina = Stamina / MaxStamina;

	//float SpeedFactor = FMath::Cos(NormalizedStamina * PI/2);
	return FMath::Lerp(MaxWanderSpeed, MaxSprintSpeed, NormalizedStamina);
}

void ALion::UpdateDebugWidget()
{
    if (DebugWidgetInstance)
    {
        // Cast the widget to access specific elements like text blocks
        UTextBlock* StaminaText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StaminaText")));
        UTextBlock* HungerText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("HungerText")));
        UTextBlock* SpeedText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("SpeedText")));
		UTextBlock* StateText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StateText")));

        if (StaminaText)
        {
            StaminaText->SetText(FText::FromString(FString::Printf(TEXT("Stamina: %.2f"), Stamina)));
        }

        if (HungerText)
        {
            HungerText->SetText(FText::FromString(FString::Printf(TEXT("Hunger: %.2f"), Hunger)));
        }

        if (SpeedText)
        {
            SpeedText->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.2f"), CalculateSpeedFromStamina())));
        }

        if (StateText)
        {
            // Convert enum value to a string
            FString StateString = StaticEnum<EAnimalState>()->GetNameStringByValue(static_cast<int64>(AnimalState));

            // Set the text of the StateText block
            StateText->SetText(FText::FromString(FString::Printf(TEXT("State: %s"), *StateString)));
        }

    }
}
