// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "MyPlayerController.h"

#include "AnomalyDrive/Player/AnomaPlayerCharacter.h"
#include "AnomalyDrive/Vehicle/VehicleBase.h"

void AMyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (ControlledPlayerCharacter == nullptr)
	{
		const auto InPawnAsAAnomaPlayerCharacter = Cast<AAnomaPlayerCharacter>(InPawn);
		check(InPawnAsAAnomaPlayerCharacter != nullptr);
		ControlledPlayerCharacter = InPawnAsAAnomaPlayerCharacter;
	}

}

void AMyPlayerController::EnterVehicle(AVehicleBase* Vehicle)
{
	ensureAlways(HasAuthority() == true);
	
	auto VehicleAsPawn = Cast<APawn>(Vehicle);
	Possess(VehicleAsPawn);
}

void AMyPlayerController::ExitVehicle(AVehicleBase* Vehicle)
{
	auto ControlledPlayerCharacterAsPawn = Cast<APawn>(ControlledPlayerCharacter);
	Possess(ControlledPlayerCharacterAsPawn);
}
