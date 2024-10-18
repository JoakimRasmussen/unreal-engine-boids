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

    DefaultStaminaDrainRate = StaminaDrainRate;
    DefaultSightRadius = SightRadius;
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

    if (GetAnimalState() == EAnimalState::EAS_Dead) return;

    DrainHunger(DeltaTime);
    if (HasStarved())
    {
        this->Die();
        return;
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

    UpdateDebugWidget();
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

    if (HasReachedLocation(CurrentWanderPoint))
    {
        CurrentWanderPoint = GetRandomPointWithinReach(800.0f, 1000.0f, 160.0f);
    }

    MoveTowardsLocation(CurrentWanderPoint, CalculateSpeedFromStamina());

    // Transition to hunting if a zebra is in sight, the lion has enough stamina, and is hungry
    // OR if the lion is desperate
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

    MoveTowardsOtherAnimal(NearestZebra, CalculateSpeedFromStamina());
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
    CurrentWanderPoint = GetRandomPointWithinReach(150.0f, 1000.0f);
}

bool ALion::ZebraInSight()
{
    if (!NearestZebra) return false;
    float DistanceToZebra = FVector::Dist(GetActorLocation(), NearestZebra->GetActorLocation());
    return DistanceToZebra < SightRadius;
}

void ALion::AttackTarget(AAnimal* Target)
{
    if (!Target || Target->GetAnimalState() == EAnimalState::EAS_Dead)
    {
        return;
    }

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
    if (OtherActor && OtherActor != this)
    {
        AAnimal* Animal = Cast<AAnimal>(OtherActor);
        if (Animal && Animal->GetAnimalType() == EAnimalType::EAT_Zebra)
        {
            if (Animal->GetAnimalState() == EAnimalState::EAS_Dead)
            {
                return;
            }

            NearestZebra = Cast<AZebra>(Animal);

            if (AttackIsValid())
            {
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
        UTextBlock* HungerText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("HungerText")));
        UTextBlock* SpeedText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("SpeedText")));
        UTextBlock* StateText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("StateText")));
        UTextBlock* DesperateText = Cast<UTextBlock>(DebugWidgetInstance->GetWidgetFromName(TEXT("DesperateText")));

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
            SpeedText->SetText(FText::FromString(FString::Printf(TEXT("Available Speed: %.2f"), CalculateSpeedFromStamina())));
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

