// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class AAnomaPlayerCharacter;
class AVehicleBase;

UCLASS()
class ANOMALYDRIVE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	
	virtual void OnPossess(APawn* InPawn) override;
	
public:
	
	void EnterVehicle(AVehicleBase* Vehicle);
	void ExitVehicle(AVehicleBase* Vehicle);
	
private:
	
	UPROPERTY() AAnomaPlayerCharacter* ControlledPlayerCharacter = nullptr;
};
