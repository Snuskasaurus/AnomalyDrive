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
enum class ECarPartBehaviour: uint8
{
	None,
	Engine,
	Wheel
};

USTRUCT(BlueprintType)
struct FCarPartBehaviour_Common
{
	GENERATED_BODY()

	// Various
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour") float Weight;
};

USTRUCT(BlueprintType)
struct FCarPartBehaviour_Wheel
{
	GENERATED_BODY()

	// Various
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour|Wheel") float Radius = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour|Wheel") FVector2f TireFriction = { 1.4f, 1.4f };
	// Brakes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour|Wheel|Brakes") bool HasBrake = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour|Wheel|Brakes") float BrakeTorque = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="CarPart|Behaviour|Wheel|Brakes") float RollingResistance = 0.01f;
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
	Brake,
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

namespace CarPartUtility
{
	static bool CarPartTypeIsCompatibleWithCarPartLocation(const ECarPartType CarPartType, const ECarPartLocation CarPartLocation)
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
			case ECarPartType::Tire:			return CarPartLocation == ECarPartLocation::WheelFrontLeft
													|| CarPartLocation == ECarPartLocation::WheelFrontRight
													|| CarPartLocation == ECarPartLocation::WheelRearLeft
													|| CarPartLocation == ECarPartLocation::WheelRearRight;
			case ECarPartType::Rim:				return CarPartLocation == ECarPartLocation::WheelFrontLeft
													|| CarPartLocation == ECarPartLocation::WheelFrontRight
													|| CarPartLocation == ECarPartLocation::WheelRearLeft
													|| CarPartLocation == ECarPartLocation::WheelRearRight;
			case ECarPartType::Brake:			return CarPartLocation == ECarPartLocation::WheelFrontLeft
													|| CarPartLocation == ECarPartLocation::WheelFrontRight
													|| CarPartLocation == ECarPartLocation::WheelRearLeft
													|| CarPartLocation == ECarPartLocation::WheelRearRight;
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
	
	static ECarPartLocation FNameToCarPartLocation(const FName& Name)
	{
		if (Name == "Engine") 				return ECarPartLocation::Engine;
		if (Name == "Battery") 				return ECarPartLocation::Battery;
		if (Name == "SteeringWheel") 		return ECarPartLocation::SteeringWheel;
		if (Name == "SeatFrontLeft") 		return ECarPartLocation::SeatFrontLeft;
		if (Name == "SeatRearLeft") 		return ECarPartLocation::SeatRearLeft;
		if (Name == "SeatFrontRight") 		return ECarPartLocation::SeatFrontRight;
		if (Name == "SeatRearRight") 		return ECarPartLocation::SeatRearRight;
		if (Name == "WheelFrontLeft") 		return ECarPartLocation::WheelFrontLeft;
		if (Name == "WheelRearLeft") 		return ECarPartLocation::WheelRearLeft;
		if (Name == "WheelFrontRight") 		return ECarPartLocation::WheelFrontRight;
		if (Name == "WheelRearRight") 		return ECarPartLocation::WheelRearRight;
		if (Name == "Exhaust") 				return ECarPartLocation::Exhaust;
		if (Name == "GearStick") 			return ECarPartLocation::GearStick;
		if (Name == "HoodFront") 			return ECarPartLocation::HoodFront;
		if (Name == "HoodRear") 			return ECarPartLocation::HoodRear;
		if (Name == "PedalGas") 			return ECarPartLocation::PedalGas;
		if (Name == "PedalClutch") 			return ECarPartLocation::PedalClutch;
		if (Name == "PedalBrake") 			return ECarPartLocation::PedalBrake;
		if (Name == "Radiator") 			return ECarPartLocation::Radiator;
		if (Name == "DoorFrontLeft") 		return ECarPartLocation::DoorFrontLeft;
		if (Name == "DoorRearLeft") 		return ECarPartLocation::DoorRearLeft;
		if (Name == "DoorFrontRight") 		return ECarPartLocation::DoorFrontRight;
		if (Name == "DoorRearRight") 		return ECarPartLocation::DoorRearRight;

		return ECarPartLocation::None; // fallback
	}

	static FName CarPartLocationToFName(ECarPartLocation CarPartLocation)
	{
		static const UEnum* EnumPtr = StaticEnum<ECarPartLocation>();
		if (!EnumPtr) return NAME_None;
		const FString NameString = EnumPtr->GetNameStringByValue(static_cast<int64>(CarPartLocation));
		return FName(*NameString);
	}

	static bool IsCarPartLocationModifyingWheelBehaviour(ECarPartLocation CarPartLocation)
	{
		if (CarPartLocation == ECarPartLocation::WheelFrontLeft
		 || CarPartLocation == ECarPartLocation::WheelRearLeft
		 || CarPartLocation == ECarPartLocation::WheelFrontRight
		 || CarPartLocation == ECarPartLocation::WheelRearRight)
		{
			return true;
		}
		return false;
	}
}

USTRUCT(BlueprintType)
struct FCarPartStatus
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CurrentDurability = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaximumDurability = 100.0f;
};