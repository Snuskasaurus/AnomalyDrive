// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleSystemBase.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "VehicleBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AAnomaPlayerCharacter;
class AAnomaItemCarPart;

USTRUCT(BlueprintType)
struct FAvailableCarPartHolder
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECarPartLocation CarPartLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SocketName;
};

USTRUCT(BlueprintType)
struct FCarPartHolder
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) AAnomaItemCarPart* CarPartActor = nullptr;
};

UCLASS(Blueprintable, Abstract)
class ANOMALYDRIVE_API AVehicleBase : public AVehicleSystemBase
{
	GENERATED_BODY()

public:
	AVehicleBase();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable) void Look(const FVector2D& LookAxisVector);
	UFUNCTION(BlueprintCallable) void Exit();
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasInstalledCarPart(ECarPartLocation CarPartLocation) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ECommonCarPartResult CanInstallCarPart(ECarPartLocation CarPartLocation, const AAnomaItemCarPart* CarPartActor) const;
	UFUNCTION(BlueprintCallable) void InstallCarPart(ECarPartLocation CarPartLocation, AAnomaItemCarPart* CarPartActor);
	UFUNCTION(BlueprintCallable) void InteractWithCarPart(AAnomaPlayerCharacter* Player, ECarPartLocation CarPartLocation);

protected:
	UFUNCTION(BlueprintImplementableEvent) void BPE_OnWheelPartChanged(ECarPartLocation CarPartLocation, FCarPartBehaviour_Wheel CarWheelPartBehaviour);
private:
	void OnWheelPartChanged(ECarPartLocation CarPartLocation);
	
private:
	FName FindSocketNameFromCarPartLocation(ECarPartLocation CarPartLocation) const; 
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FAvailableCarPartHolder> AvailableCarParts;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TMap<ECarPartLocation, FCarPartHolder> InstalledCarParts;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) UCameraComponent* InteriorPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) UCameraComponent* ExteriorPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) USpringArmComponent* ExteriorCameraBoom;
};
