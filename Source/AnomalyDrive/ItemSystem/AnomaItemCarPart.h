// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomaItem.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "AnomalyDrive/ItemSystem/AnomaItem.h"
#include "GameFramework/Actor.h"
#include "AnomaItemCarPart.generated.h"

UCLASS(BlueprintType, Abstract)
class AAnomaItemCarPart : public AAnomaItem
{
	GENERATED_BODY()

public:
	AAnomaItemCarPart();

public:
	const FCarPartDesc& GetCarCarPartDesc() const { return CarPartDesc; };
	const FCarPartStatus& GetCarPartStatus() const { return CarPartStatus; };
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCarPartDesc CarPartDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCarPartStatus CarPartStatus;
};
