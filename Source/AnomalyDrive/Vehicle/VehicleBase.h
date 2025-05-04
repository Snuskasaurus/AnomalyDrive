// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleSystemBase.h"
#include "AnomalyDrive/CarPartSystem/AnomaCarPart.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "VehicleBase.generated.h"

USTRUCT(BlueprintType)
struct FCarPartHolder
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) AAnomaCarPart* CarPartActor = nullptr;
};

UCLASS(Blueprintable, Abstract)
class ANOMALYDRIVE_API AVehicleBase : public AVehicleSystemBase
{
	GENERATED_BODY()

public:
	AVehicleBase();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasInstalledCarPart(ECarPartLocation CarPartLocation) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ECommonCarPartResult CanInstallCarPart(ECarPartLocation CarPartLocation, const AAnomaCarPart* CarPartActor) const;
	UFUNCTION(BlueprintCallable) void InstallCarPart(ECarPartLocation CarPartLocation, AAnomaCarPart* CarPartActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<ECarPartLocation> AvailableCarParts;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TMap<ECarPartLocation, FCarPartHolder> InstalledCarParts;
};
