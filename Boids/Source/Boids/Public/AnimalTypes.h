#pragma once

UENUM(BlueprintType)
enum class EAnimalType : uint8
{
    EAT_Lion UMETA(DisplayName = "Lion"),
    EAT_Zebra UMETA(DisplayName = "Zebra"),
    EAT_Other UMETA(DisplayName = "Other")
};
