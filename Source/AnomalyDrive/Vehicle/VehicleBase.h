// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleSystemBase.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "VehicleBase.generated.h"

class UBoxComponent;
class USpringArmComponent;
class UCameraComponent;
class AAnomaPlayerCharacter;
class AAnomaItemCarPart;

UENUM(BlueprintType, meta=(Bitflags))
enum class EWheelState : uint8
{
	None  = 0        UMETA(DisplayName = "None"),
	Brake = 1 << 0   UMETA(DisplayName = "Brake"),
	Rim   = 1 << 1   UMETA(DisplayName = "Rim"),
	Tire  = 1 << 2   UMETA(DisplayName = "Tire"),
};
ENUM_CLASS_FLAGS(EWheelState)

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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECarPartLocation CarPartLocation = ECarPartLocation::None;
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
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasInstalledSpecificCarPart(const ECarPartLocation CarPartLocation, const ECarPartType CarPartType) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) bool HasInstalledCarPart(const ECarPartLocation CarPartLocation) const;
	UFUNCTION(BlueprintCallable, BlueprintPure) ECommonCarPartResult CanInstallCarPart(ECarPartLocation CarPartLocation, const AAnomaItemCarPart* CarPartActor) const;
	UFUNCTION(BlueprintCallable) void InstallCarPart(ECarPartLocation CarPartLocation, AAnomaItemCarPart* CarPartActor);
	UFUNCTION(BlueprintCallable) void InteractWithCarPart(AAnomaPlayerCharacter* Player, ECarPartLocation CarPartLocation);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta=(Bitmask="WheelStateBitMask", BitmaskEnum="EWheelState"))
	void BPE_OnWheelPartChanged(ECarPartLocation CarPartLocation, FCarPartBehaviour_Wheel CarWheelPartBehaviour, int32 WheelStateBitMask);
private:
	void OnWheelPartChanged(ECarPartLocation CarPartLocation);
	
private:
	FName FindSocketNameFromCarPartLocation(ECarPartLocation CarPartLocation) const; 
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FAvailableCarPartHolder> AvailableCarParts;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FCarPartHolder> InstalledCarParts;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) UCameraComponent* InteriorPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) UCameraComponent* ExteriorPersonCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera) USpringArmComponent* ExteriorCameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<UBoxComponent*> InteractableComponents;
};
