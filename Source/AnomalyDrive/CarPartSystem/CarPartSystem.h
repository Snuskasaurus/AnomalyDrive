#pragma once

#include "CoreMinimal.h"
#include "CarPartSystem.generated.h"

UENUM(BlueprintType)
enum class ECarPartType: uint8
{
	None,
	Engine,
	Battery,
	SteeringWheel,
	Seat,
	Tire,
	Rim,
	Exhaust,
	GearStick,
	HoodFront,
	HoodRear,
	Pedal,
	Radiator,
	DoorFrontLeft,
	DoorRearLeft,
	DoorFrontRight,
	DoorRearRight,
};

UENUM(BlueprintType)
enum class ECarPartLocation : uint8
{
	None,
	Engine,
	Battery,
	SteeringWheel,
	SeatFrontLeft,
	SeatRearLeft,
	SeatFrontRight,
	SeatRearRight,
	WheelFrontLeft,
	WheelRearLeft,
	WheelFrontRight,
	WheelRearRight,
	Exhaust,
	GearStick,
	HoodFront,
	HoodRear,
	PedalGas,
	PedalClutch,
	PedalBrake,
	Radiator,
	DoorFrontLeft,
	DoorRearLeft,
	DoorFrontRight,
	DoorRearRight,
};

USTRUCT(BlueprintType)
struct FCarPartDesc
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECarPartType CarPartType = ECarPartType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* StaticMesh = nullptr;
};

USTRUCT(BlueprintType)
struct FCarPartStatus
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CurrentDurability = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaximumDurability = 100.0f;
};