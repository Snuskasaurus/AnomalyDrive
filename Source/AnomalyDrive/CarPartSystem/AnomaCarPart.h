// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CarPartSystem.h"
#include "GameFramework/Actor.h"
#include "AnomaCarPart.generated.h"

UCLASS(BlueprintType, Abstract)
class AAnomaCarPart : public AActor
{
	GENERATED_BODY()

public:
	AAnomaCarPart();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCarPartDesc CarPartDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCarPartStatus CarPartStatus;
};
