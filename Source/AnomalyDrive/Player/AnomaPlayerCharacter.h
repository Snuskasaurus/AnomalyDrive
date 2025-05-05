// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "AnomalyDrive/ItemSystem/AnomaItemCarPart.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AnomaPlayerCharacter.generated.h"

class AAnomaItem;
enum class ECarPartLocation : uint8;
class UBoxComponent;
class AVehicleBase;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AAnomaPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AAnomaPlayerCharacter();

public: /// Components accessors ---------------------------------------------------------------------------------------
	
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected: /// Unreal overrides ----------------------------------------------------------------------------------------

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginReplication() override;
	virtual void NotifyControllerChanged() override;
	
protected: /// Inputs functions ----------------------------------------------------------------------------------------

	UFUNCTION(BlueprintCallable)
	void Move(const FVector2D& MovementVector);

	UFUNCTION(BlueprintCallable)
	void Look(const FVector2D& LookAxisVector);
	
	UFUNCTION(BlueprintCallable)
	void DropItem();
	
	UFUNCTION(BlueprintCallable)
	void Interact();

	UFUNCTION(BlueprintCallable)
	void StartVehicleModification();
	
	UFUNCTION(BlueprintCallable)
	void EndVehicleModification();
	
private: /// Private Items and interactions functions ------------------------------------------------------------------

	void TickInteractionTrace(float DeltaSeconds);
	void PutItemInHand(AAnomaItem* Item);
	void DropItemInHand();
	void InteractWithVehicleCarPart(AVehicleBase* Vehicle, UBoxComponent* CarPartCollider);
	void UseVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation);
	void InstallVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation, AAnomaItemCarPart* CarPart);
	
	void CancelVehicleModification();
	void CleanVehicleModification();
	UFUNCTION() void InstallCarPartCompleted();
	UFUNCTION() void UninstallCarPartCompleted();
	
protected: /// Components ----------------------------------------------------------------------------------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh)
	UStaticMeshComponent* MeshItemInHand;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	UCameraComponent* FirstPersonCameraComponent;

protected: /// Items and interactions variables ------------------------------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionRange = 160.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionRadius = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ETraceTypeQuery> InteractionTraceQuery = ETraceTypeQuery::TraceTypeQuery3;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int InventoryIndexInHand = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AAnomaItem*> ItemInInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FHitResult> HitResultInteraction;

	bool IsModifyingVehicle = false;
	FTimerHandle TimerHandleVehicleModification;
	UPROPERTY() AVehicleBase* VehicleAimedForModification = nullptr;
	UPROPERTY() ECarPartLocation VehicleLocationAimedForModification = ECarPartLocation::None;
	
};

