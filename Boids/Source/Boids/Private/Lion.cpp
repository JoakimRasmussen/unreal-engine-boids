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
    AttackSphere->InitSphereRadius(200.0f);
    AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ALion::OnAttackSphereOverlap);

	// Set the collision properties of the attack sphere
    AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackSphere->SetCollisionObjectType(ECC_WorldDynamic);
    AttackSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // Overlap with characters (zebra)
    AttackSphere->SetGenerateOverlapEvents(true);


    // Set default animal type and initial state
    AnimalType = EAnimalType::EAT_Lion;
    AnimalState = EAnimalState::EAS_Wandering;

    DefaultStaminaDrainRate = StaminaDrainRate;
    DefaultSightRadius = SightRadius;
	bDebugMode = true;
}

// Lifecycle Functions
void ALion::BeginPlay()
{
    Super::BeginPlay();

    Stamina = MaxStamina; // Initialize stamina
    Hunger = MaxHunger; // Initialize hunger
    StaminaDrainRate = DefaultStaminaDrainRate; // Initialize stamina drain rate
    SightRadius = DefaultSightRadius; // Initialize sight radius

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
	Speed = GetVelocity().Size();
    UpdateDebugWidget();

    if (GetAnimalState() == EAnimalState::EAS_Dead) return;

    DrainHunger(DeltaTime);
    if (HasStarved())
    {
        this->Die();
        return;
    }

    if (bDebugMode)
    {
        // Draw a debug sphere representing the lion's sight radius
        DrawDebugSphere(GetWorld(), GetActorLocation(), SightRadius, 24, FColor::Red, false, -1.0f, 0, 2.0f);

		// Draw a debug sphere representing the lion's attack radius
        FVector SphereLocation = AttackSphere->GetComponentLocation();
		DrawDebugSphere(GetWorld(), SphereLocation, AttackSphere->GetUnscaledSphereRadius(), 24, FColor::Green, false, -1.0f, 0, 2.0f);

        // Draw a debug sphere for the CurrentWanderPoint
        DrawDebugSphere(GetWorld(), CurrentWanderPoint, 10.0f, 12, FColor::Blue, false, -1.0f, 0, 2.0f);
    }

    HandleDesperation();

    switch (GetAnimalState())
    {
    case EAnimalState::EAS_Resting:
    {
        HandleRestingState(DeltaTime);
        break;
    }

    case EAnimalState::EAS_Wandering:
    {
        HandleWanderingState(DeltaTime);
        break;
    }

    case EAnimalState::EAS_Hunting:
    {
        HandleHuntingState(DeltaTime);
        break;
    }

    case EAnimalState::EAS_Attacking:
    {
        HandleAttackingState(DeltaTime);
        break;
    }

    default:
        break;
    }
}

// State Transition Functions
void ALion::StartHunting()
{
    AnimalState = EAnimalState::EAS_Hunting;
}

void ALion::EndAttack()
{
    GetCharacterMovement()->StopMovementImmediately();
    AnimalState = EAnimalState::EAS_Wandering;
    NearestZebra = nullptr;
    bIsAttacking = false;
}

// Must be in hunting state
bool ALion::AttackIsValid()
{
    return AnimalState == EAnimalState::EAS_Hunting;
}

void ALion::HandleRestingState(float DeltaTime)
{
    RegenerateStamina(DeltaTime);
    if (ShouldExitResting())
        StartWandering();
}

void ALion::HandleWanderingState(float DeltaTime)
{
    DrainStamina(DeltaTime);
    if (NeedRest())
    {
        StartResting();
        return;
    }

    // If the lion is stuck, find an unstuck location and move towards it
    if (IsStuck())
    {
        CurrentWanderPoint = FindUnstuckLocation();
        MoveTowardsLocation(CurrentWanderPoint, CalculateSpeedFromStamina());
        return;  // Exit early if we moved to a new unstuck position
    }

    // If the destination is reached, choose a new random point
    else if (HasReachedLocation(CurrentWanderPoint))
    {
        CurrentWanderPoint = GetRandomPointWithinReach(800.0f, 1000.0f, 160.0f);
    }
    // If the path to the current wander point is blocked, choose a new nearby point
    else if (IsPathBlocked(CurrentWanderPoint))
    {
        CurrentWanderPoint = GetRandomPointWithinReach(100.0f, 200.0f, 240.0f, true);
    }

    // Move towards the current or new wander point if not stuck
    MoveTowardsLocation(CurrentWanderPoint, CalculateSpeedFromStamina());

    // Transition to hunting if conditions are met
    if (ZebraInSight() && (EnoughStamina() && IsHungry()) || IsDesperate())
    {
        StartHunting();
    }
}


void ALion::HandleHuntingState(float DeltaTime)
{
    DrainStamina(DeltaTime);

    // Check if the lion should stop hunting (unless it is desperate)
    if (!IsDesperate() && (!ZebraInSight() || !EnoughStamina() || !IsHungry()))
    {
        StartWandering();
        return;
    }

    // If the lion is stuck, find an unstuck location and move towards it
    if (IsStuck())
    {
        MoveTowardsLocation(FindUnstuckLocation(), CalculateSpeedFromStamina());
        return;
    }

    // If the path to the zebra is blocked but the lion is not stuck, choose a nearby point
    else if (IsPathBlocked(NearestZebra->GetActorLocation()))
    {
		FVector NearbyPoint = GetRandomPointNear(NearestZebra->GetActorLocation(), 200.0f, 800.0f);
        MoveTowardsLocation(NearbyPoint, CalculateSpeedFromStamina());
    }
    // Move directly towards the zebra if the path is clear
    else
    {
        MoveTowardsOtherAnimal(NearestZebra, CalculateSpeedFromStamina());
    }
}



void ALion::HandleAttackingState(float DeltaTime)
{
    if (NearestZebra && !bIsAttacking)
    {
        MoveTowardsOtherAnimal(NearestZebra, CalculateSpeedFromStamina());
        AttackTarget(NearestZebra);
    }
}

// Updates variables like sight radius and stamina drain rate
void ALion::HandleDesperation()
{
    if (IsDesperate())
    {
        SightRadius = DesperateSightRadius;
        StaminaDrainRate = DesperateStaminaDrainRate;
    }
    else
    {
        SightRadius = DefaultSightRadius;
        StaminaDrainRate = DefaultStaminaDrainRate;
    }
}

bool ALion::ShouldExitResting()
{
    return Stamina >= MaxStamina || IsDesperate();
}

void ALion::StartWandering()
{
    AnimalState = EAnimalState::EAS_Wandering;
    CurrentWanderPoint = GetRandomPointWithinReach(150.0f, 1000.0f, 180.0f);
}

bool ALion::ZebraInSight()
{
    if (!NearestZebra) return false;
    float DistanceToZebra = FVector::Dist(GetActorLocation(), NearestZebra->GetActorLocation());
    return DistanceToZebra < SightRadius;
}

FVector ALion::FindUnstuckLocation(float ScanRadius, int32 ScanSteps)
{
    // Scan in a circle around the animal to find an open path
    float AngleStep = 360.0f / ScanSteps;
    FVector BestLocation;
    float ClosestDistance = FLT_MAX;
    bool bFoundValidDirection = false;

    for (int32 Step = 0; Step < ScanSteps; ++Step)
    {
        // Calculate the direction for the current angle
        float Angle = FMath::DegreesToRadians(Step * AngleStep);
        FVector Direction = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f);
        FVector PotentialLocation = GetActorLocation() + Direction * ScanRadius;

        // Check if this potential location is not blocked
        if (!IsPathBlocked(PotentialLocation))
        {
            // If hunting, prioritize paths closer to the target (zebra)
            if (AnimalState == EAnimalState::EAS_Hunting && NearestZebra)
            {
                float DistanceToZebra = FVector::Dist(PotentialLocation, NearestZebra->GetActorLocation());
                if (DistanceToZebra < ClosestDistance)
                {
                    BestLocation = PotentialLocation;
                    ClosestDistance = DistanceToZebra;
                    bFoundValidDirection = true;
                }
            }
            else
            {
                // If not in hunting state, take the first valid location
                BestLocation = PotentialLocation;
                bFoundValidDirection = true;
                break;  // Exit early as we found a suitable point
            }
        }
    }

    // If a valid location is found, return it
    if (bFoundValidDirection)
    {
        return BestLocation;
    }

    // If all directions are blocked, return a random nearby point as a last resort
    return GetRandomPointWithinReach(100.0f, 200.0f, 360.0f);
}



void ALion::AttackTarget(AAnimal* Target)
{
    if (!Target || Target->GetAnimalState() == EAnimalState::EAS_Dead)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Lion attacking"));

    bIsAttacking = true;

    // Look at the zebra
    FVector LionLocation = GetActorLocation();
    FVector TargetLocation = Target->GetActorLocation();
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(LionLocation, TargetLocation);
    SetActorRotation(LookAtRotation);

    Target->Die();  // Kill the zebra

    Hunger = MaxHunger;  // Reset hunger

    GetWorldTimerManager().SetTimer(AttackEndTimer, this, &ALion::EndAttack, AttackCooldown, false);
}

// Collision Handling
void ALion::OnAttackSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Overlap detected with another actor."));
    if (OtherActor && OtherActor != this)
    {
        UE_LOG(LogTemp, Warning, TEXT("Actor is valid and not self."));
        AAnimal* Animal = Cast<AAnimal>(OtherActor);
        if (Animal && Animal->GetAnimalType() == EAnimalType::EAT_Zebra)
        {
            UE_LOG(LogTemp, Warning, TEXT("Lion found zebra."));
            if (Animal->GetAnimalState() == EAnimalState::EAS_Dead)
            {
                UE_LOG(LogTemp, Warning, TEXT("Zebra is dead. Ignoring."));
                return;
            }

            NearestZebra = Cast<AZebra>(Animal);

            if (AttackIsValid())
            {
                UE_LOG(LogTemp, Warning, TEXT("Attack is valid. Changing state to Attacking."));
                AnimalState = EAnimalState::EAS_Attacking;
            }
        }
    }

}

// Drain Stamina, which is affected by the current state
void ALion::DrainStamina(float DeltaTime)
{
    float DrainRateMultiplier = (AnimalState == EAnimalState::EAS_Wandering) ? WanderingStaminaDrainMultiplier :
        (AnimalState == EAnimalState::EAS_Hunting) ? SprintingStaminaDrainMultiplier : 1.0f;

    Stamina = FMath::Clamp(Stamina - StaminaDrainRate * DeltaTime * DrainRateMultiplier, 0.0f, MaxStamina);
}

void ALion::RegenerateStamina(float DeltaTime)
{
    Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.0f, MaxStamina);
}

// Hunger drain is affected by the current state
void ALion::DrainHunger(float DeltaTime)
{
    float DrainRateMultiplier = (AnimalState == EAnimalState::EAS_Resting) ? RestingHungerDrainMultiplier :
        (AnimalState == EAnimalState::EAS_Hunting) ? HuntingHungerDrainMultiplier : 1.0f;

    Hunger = FMath::Clamp(Hunger - HungerDrainRate * DrainRateMultiplier * DeltaTime, 0.0f, MaxHunger);
}

float ALion::CalculateSpeedFromStamina()
{
    float NormalizedStamina = Stamina / MaxStamina;
    float LowerBound = (AnimalState == EAnimalState::EAS_Hunting) ? MaxWalkingSpeed : MinWalkingSpeed;
    float UpperBound = (AnimalState == EAnimalState::EAS_Hunting) ? MaxSprintSpeed : MaxWalkingSpeed;

    float SpeedFactor = FMath::Cos(NormalizedStamina * PI / 2 - PI / 2); // Cosine interpolation
    return FMath::Lerp(LowerBound, UpperBound, SpeedFactor);
}

void ALion::UpdateDebugWidget()
{
    if (DebugWidgetInstance)
    {
        // Cast the widget to access specific elements like text blocks
        UTextBlock* StaminaText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StaminaText")));
		UTextBlock* StaminaThresholdText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StaminaThresholdText")));
        UTextBlock* HungerText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("HungerText")));
		UTextBlock* HungerThresholdText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("HungerThresholdText")));
        UTextBlock* SpeedText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("SpeedText")));
		UTextBlock* ActualSpeedText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("ActualSpeedText")));
        UTextBlock* StateText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StateText")));
        UTextBlock* DesperateText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("DesperateText")));
		UTextBlock* IsStuckText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("IsStuckText")));

        if (StaminaText)
        {
            StaminaText->SetText(FText::FromString(FString::Printf(TEXT("Stamina: %.2f"), Stamina)));
        }

		if (StaminaThresholdText)
		{
			StaminaThresholdText->SetText(FText::FromString(FString::Printf(TEXT("| %.2f"), StaminaHuntThreshold * MaxStamina)));
		}

        if (HungerText)
        {
            HungerText->SetText(FText::FromString(FString::Printf(TEXT("Hunger: %.2f"), Hunger)));
        }

		if (HungerThresholdText)
		{
			HungerThresholdText->SetText(FText::FromString(FString::Printf(TEXT("| %.2f"), HungerThreshold * MaxHunger)));
		}

        if (SpeedText)
        {
            SpeedText->SetText(FText::FromString(FString::Printf(TEXT("| %.2f"), CalculateSpeedFromStamina())));
        }

		if (ActualSpeedText)
		{
			ActualSpeedText->SetText(FText::FromString(FString::Printf(TEXT("Speed: %.2f"), Speed)));
		}

        if (StateText)
        {
            // Convert enum value to a string
            FString StateString = StaticEnum<EAnimalState>()->GetNameStringByValue(static_cast<int64>(AnimalState));

            // Set the text of the StateText block
            StateText->SetText(FText::FromString(FString::Printf(TEXT("State: %s"), *StateString)));
        }

        if (DesperateText)
        {
            DesperateText->SetText(FText::FromString(IsDesperate() ? "Desperate" : "Not Desperate"));
        }

        if (IsStuckText)
        {
            IsStuckText->SetText(FText::FromString(IsStuck() ? "Stuck" : "Not Stuck"));
        }

    }
}

// Enough stamina to hunt
bool ALion::EnoughStamina()
{
    return Stamina > StaminaHuntThreshold * MaxStamina;
}

// Is desperate for food
bool ALion::IsDesperate()
{
    return Hunger <= DesperateHungerThreshold * MaxHunger;
}

