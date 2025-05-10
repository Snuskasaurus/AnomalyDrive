// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "VehicleBase.h"
#include "Components/BoxComponent.h"
#include "AnomalyDrive/ItemSystem/AnomaItemCarPart.h"
#include "AnomalyDrive/Player/AnomaPlayerCharacter.h"
#include "AnomalyDrive/Player/MyPlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
AVehicleBase::AVehicleBase()
{
	// Create a Interior CameraComponent
	{
		InteriorPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("InteriorCamera"));
		InteriorPersonCameraComponent->SetupAttachment(VehicleMesh);
		InteriorPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Position the camera
		InteriorPersonCameraComponent->bUsePawnControlRotation = true;
	}
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	{
		ExteriorCameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		ExteriorCameraBoom->SetupAttachment(VehicleMesh);
		ExteriorCameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
		ExteriorCameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	}
	
	// Create a Exterior CameraComponent
	{
		ExteriorPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ExteriorCamera"));
		ExteriorPersonCameraComponent->SetupAttachment(ExteriorCameraBoom);
		ExteriorPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Position the camera
		ExteriorPersonCameraComponent->bUsePawnControlRotation = true;
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::BeginPlay()
{
	Super::BeginPlay();
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::Look(const FVector2D& LookAxisVector)
{
	if (Controller == nullptr)
		return;
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::Exit()
{
	auto MyPlayerController = Cast<AMyPlayerController>(this->GetController());
	MyPlayerController->ExitVehicle(this);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
bool AVehicleBase::HasInstalledCarPart(const ECarPartLocation CarPartLocation) const
{
	
	for (auto InstalledCarPartElement : InstalledCarParts)
	{
		if (InstalledCarPartElement.CarPartLocation == CarPartLocation)
		{
			if (InstalledCarPartElement.CarPartActor != nullptr)
				return true;
			
			return false;
		}
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
bool AVehicleBase::HasInstalledSpecificCarPart(const ECarPartLocation CarPartLocation, const ECarPartType CarPartType) const
{
	for (auto InstalledCarPartElement : InstalledCarParts)
	{
		if (InstalledCarPartElement.CarPartLocation == CarPartLocation)
		{
			if (InstalledCarPartElement.CarPartActor != nullptr)
			{
				if (InstalledCarPartElement.CarPartActor->GetItemCarPartDesc().CarPartType == CarPartType)
					return true;
			}
			
			return false;
		}
	}
	return false;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
ECommonCarPartResult AVehicleBase::CanInstallCarPart(ECarPartLocation CarPartLocation, const AAnomaItemCarPart* CarPartActor) const
{
	const auto CarPartDesc = CarPartActor->GetItemCarPartDesc();
	
	if (CarPartUtility::CarPartTypeIsCompatibleWithCarPartLocation(CarPartDesc.CarPartType, CarPartLocation) == false)
	{
		return ECommonCarPartResult::Install_IncompatibleCarPartLocation;
	}
	
	if (HasInstalledSpecificCarPart(CarPartLocation, CarPartDesc.CarPartType) == true)
	{
		return ECommonCarPartResult::Install_CarPartLocationNotFree;
	}
	
	return ECommonCarPartResult::Success;
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::InstallCarPart(ECarPartLocation CarPartLocation, AAnomaItemCarPart* CarPartActor)
{
#if !UE_BUILD_SHIPPING
	ensureAlways(CanInstallCarPart(CarPartLocation, CarPartActor) == ECommonCarPartResult::Success);
#endif

	FCarPartHolder CarPartHolder;
	CarPartHolder.CarPartActor = CarPartActor;
	CarPartHolder.CarPartLocation = CarPartLocation;
	InstalledCarParts.Add(CarPartHolder);

	UStaticMeshComponent* CarPartMesh = NewObject<UStaticMeshComponent>(this);
	CarPartMesh->SetStaticMesh(CarPartActor->GetItemCarPartDesc().StaticMesh);
	CarPartMesh->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FindSocketNameFromCarPartLocation(CarPartLocation));
	CarPartMesh->RegisterComponent();
	CarPartMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	if (CarPartUtility::IsCarPartLocationModifyingWheelBehaviour(CarPartLocation) == true)
	{
		OnWheelPartChanged(CarPartLocation);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
void AVehicleBase::InteractWithCarPart(AAnomaPlayerCharacter* Player, ECarPartLocation CarPartLocation)
{
	if (CarPartLocation == ECarPartLocation::SeatFrontLeft)
	{
		auto MyPlayerController = Cast<AMyPlayerController>(Player->GetController());
		check(MyPlayerController);
		MyPlayerController->EnterVehicle(this);
	}
}
void AVehicleBase::OnWheelPartChanged(const ECarPartLocation CarPartLocation)
{
	FCarPartBehaviour_Wheel FinalWheelBehaviour;
	int32 WheelStateBitMask = static_cast<int32>(EWheelState::None);

	for (const auto Element : InstalledCarParts)
	{
		if (Element.CarPartLocation != CarPartLocation)
			continue;

		const auto CarPartDesc = Element.CarPartActor->GetItemCarPartDesc();

		switch (CarPartDesc.CarPartType)
		{
			case ECarPartType::Tire:
			{
				WheelStateBitMask |= static_cast<int32>(EWheelState::Tire);
			} break;
			case ECarPartType::Brake:
			{
				WheelStateBitMask |= static_cast<int32>(EWheelState::Brake);
			} break;
			case ECarPartType::Rim:
			{
				WheelStateBitMask |= static_cast<int32>(EWheelState::Rim);
			} break;
		}

		const auto NewItemWheelBehaviour = CarPartDesc.WheelCarPartBehaviour;

		// Take the maximum values
		FinalWheelBehaviour.Radius = FMath::Max(FinalWheelBehaviour.Radius, NewItemWheelBehaviour.Radius);
		FinalWheelBehaviour.TireFriction = FMath::Max(FinalWheelBehaviour.TireFriction, NewItemWheelBehaviour.TireFriction);

		// True if at least 1 true
		FinalWheelBehaviour.HasBrake = FinalWheelBehaviour.HasBrake || NewItemWheelBehaviour.HasBrake;

		// Increment
		FinalWheelBehaviour.BrakeTorque += FinalWheelBehaviour.BrakeTorque;
		FinalWheelBehaviour.RollingResistance += NewItemWheelBehaviour.RollingResistance;
	}
	
	BPE_OnWheelPartChanged(CarPartLocation, FinalWheelBehaviour, WheelStateBitMask);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
FName AVehicleBase::FindSocketNameFromCarPartLocation(ECarPartLocation CarPartLocation) const
{
	for (const FAvailableCarPartHolder& AvailableCarPartHolder : AvailableCarParts)
	{
		if (AvailableCarPartHolder.CarPartLocation == CarPartLocation)
		{
			return AvailableCarPartHolder.SocketName;
		}
	}
	return TEXT("");
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
