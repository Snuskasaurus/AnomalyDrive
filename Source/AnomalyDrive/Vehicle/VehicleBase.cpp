// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "VehicleBase.h"
#include "Components/BoxComponent.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
AVehicleBase::AVehicleBase()
{
}

void AVehicleBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Create the car parts colliders components
	{
		for (const FAvailableCarPartHolder& AvailableCarPart : AvailableCarParts)
		{
			UBoxComponent* BoxComponent = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(), NAME_None, RF_Transactional);
			BoxComponent->SetupAttachment(VehicleMesh, AvailableCarPart.SocketName);
			BoxComponent->RegisterComponent();
			BoxComponent->SetCollisionProfileName(TEXT("Interactable"));
			BoxComponent->InitBoxExtent(FVector(10.0f));
			BoxComponent->ComponentTags.Add(AvailableCarPart.SocketName);
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
ECommonCarPartResult AVehicleBase::CanInstallCarPart(ECarPartLocation CarPartLocation, const AAnomaCarPart* CarPartActor) const
{
	const auto CarPartDesc = CarPartActor->GetCarCarPartDesc();
	
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
void AVehicleBase::InstallCarPart(ECarPartLocation CarPartLocation, AAnomaCarPart* CarPartActor)
{
#if !UE_BUILD_SHIPPING
	ensureAlways(CanInstallCarPart(CarPartLocation, CarPartActor) == ECommonCarPartResult::Success);
#endif

	FCarPartHolder CarPartHolder;
	CarPartHolder.CarPartActor = CarPartActor;
	
	InstalledCarParts.Add(CarPartLocation, CarPartHolder);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------
