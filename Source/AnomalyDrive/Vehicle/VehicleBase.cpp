// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "VehicleBase.h"
#include "Components/BoxComponent.h"
#include "AnomalyDrive/ItemSystem/AnomaItemCarPart.h"
#include "AnomalyDrive/Player/AnomaPlayerCharacter.h"
#include "AnomalyDrive/Player/MyPlayerController.h"
#include "Camera/CameraComponent.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
AVehicleBase::AVehicleBase()
{
	// Create a CameraComponent
	{
		FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
		FirstPersonCameraComponent->SetupAttachment(VehicleMesh);
		FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Position the camera
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
	}
}

void AVehicleBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Create the car parts colliders components
	{
		for (const FAvailableCarPartHolder& AvailableCarPart : AvailableCarParts)
		{
			FName Name = CarPartUtility::CarPartLocationToFName(AvailableCarPart.CarPartLocation);
			
			UBoxComponent* BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), Name, RF_Transactional);
			BoxComponent->SetupAttachment(VehicleMesh, AvailableCarPart.SocketName);
			BoxComponent->RegisterComponent();
			BoxComponent->SetCollisionProfileName(TEXT("Interactable"));
			BoxComponent->InitBoxExtent(FVector(10.0f));
			BoxComponent->ComponentTags.Add(Name);
			BoxComponent->bVisualizeComponent = true;
			this->AddInstanceComponent(BoxComponent);
		}
	}
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
bool AVehicleBase::HasInstalledCarPart(ECarPartLocation CarPartLocation) const
{
	for (auto InstalledCarPartElement : InstalledCarParts)
	{
		if (InstalledCarPartElement.Key == CarPartLocation)
		{
			if (InstalledCarPartElement.Value.CarPartActor != nullptr)
			{
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
	
	if (HasInstalledCarPart(CarPartLocation) == true)
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
	
	InstalledCarParts.Add(CarPartLocation, CarPartHolder);

	UStaticMeshComponent* CarPartMesh = NewObject<UStaticMeshComponent>(this);
	CarPartMesh->SetStaticMesh(CarPartActor->GetItemCarPartDesc().StaticMesh);
	CarPartMesh->AttachToComponent(VehicleMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FindSocketNameFromCarPartLocation(CarPartLocation));
	CarPartMesh->RegisterComponent();
	CarPartMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

	if (CarPartUtility::IsCarPartLocationModifyWheelBehaviour(CarPartLocation) == true)
	{
		BPE_OnWheelChanged(CarPartLocation);
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
