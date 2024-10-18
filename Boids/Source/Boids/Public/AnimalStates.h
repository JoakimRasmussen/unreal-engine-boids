#pragma once


UENUM(BlueprintType)
enum class EAnimalState : uint8
{
	EAS_Flocking UMETA(DisplayName = "Flocking"),
	EAS_Eating UMETA(DisplayName = "Eating"),
	EAS_Resting UMETA(DisplayName = "Resting"),
	EAS_Hunting UMETA(DisplayName = "Hunting"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Fleeing UMETA(DisplayName = "Fleeing"),
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_Wandering UMETA(DisplayName = "Wandering")
};