#pragma once


UENUM(BlueprintType)
enum class EAnimalState : uint8
{
	EAS_Flocking UMETA(DisplayName = "Flocking"),
	EAS_Eating UMETA(DisplayName = "Eating"),
	EAS_Resting UMETA(DisplayName = "Resting")
};