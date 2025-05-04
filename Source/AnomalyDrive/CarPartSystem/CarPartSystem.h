#pragma once

#include "CoreMinimal.h"
#include "CarPartSystem.generated.h"

UENUM(BlueprintType)
enum class ECommonCarPartResult : uint8
{
	Success,
	Install_IncompatibleCarPartLocation,
	Install_CarPartLocationNotFree,
};

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
	TireFrontLeft,
	TireRearLeft,
	TireFrontRight,
	TireRearRight,
	RimFrontLeft,
	RimRearLeft,
	RimFrontRight,
	RimRearRight,
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

namespace CarPartUtility
{
	inline bool CarPartTypeIsCompatibleWithCarPartLocation(ECarPartType CarPartType, ECarPartLocation CarPartLocation)
	{
		switch (CarPartType)
		{
			case ECarPartType::None:			return false;
			case ECarPartType::Engine:			return CarPartLocation == ECarPartLocation::Engine;
			case ECarPartType::Battery:			return CarPartLocation == ECarPartLocation::Battery;
			case ECarPartType::SteeringWheel:	return CarPartLocation == ECarPartLocation::SteeringWheel;
			case ECarPartType::Seat:			return CarPartLocation == ECarPartLocation::SeatFrontLeft
													|| CarPartLocation == ECarPartLocation::SeatFrontRight
													|| CarPartLocation == ECarPartLocation::SeatRearLeft
													|| CarPartLocation == ECarPartLocation::SeatRearRight;
			case ECarPartType::Tire:			return CarPartLocation == ECarPartLocation::TireFrontLeft
													|| CarPartLocation == ECarPartLocation::TireFrontRight
													|| CarPartLocation == ECarPartLocation::TireRearLeft
													|| CarPartLocation == ECarPartLocation::TireRearRight;
			case ECarPartType::Rim:				return CarPartLocation == ECarPartLocation::RimFrontLeft
													|| CarPartLocation == ECarPartLocation::RimFrontRight
													|| CarPartLocation == ECarPartLocation::RimRearLeft
													|| CarPartLocation == ECarPartLocation::RimRearRight;
			case ECarPartType::Exhaust:			return CarPartLocation == ECarPartLocation::Exhaust;
			case ECarPartType::GearStick:		return CarPartLocation == ECarPartLocation::GearStick;
			case ECarPartType::HoodFront:		return CarPartLocation == ECarPartLocation::HoodFront;
			case ECarPartType::HoodRear:		return CarPartLocation == ECarPartLocation::HoodRear;
			case ECarPartType::Pedal:			return CarPartLocation == ECarPartLocation::PedalBrake
													|| CarPartLocation == ECarPartLocation::PedalClutch
													|| CarPartLocation == ECarPartLocation::PedalGas;
			case ECarPartType::Radiator:		return CarPartLocation == ECarPartLocation::Radiator;
			case ECarPartType::DoorFrontLeft:	return CarPartLocation == ECarPartLocation::DoorFrontLeft;
			case ECarPartType::DoorRearLeft:	return CarPartLocation == ECarPartLocation::DoorRearLeft;
			case ECarPartType::DoorFrontRight:	return CarPartLocation == ECarPartLocation::DoorFrontRight;
			case ECarPartType::DoorRearRight:	return CarPartLocation == ECarPartLocation::DoorRearRight;
		}
		return false;
	}
}

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