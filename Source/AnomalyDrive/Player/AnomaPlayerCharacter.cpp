// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnomaPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "KismetTraceUtils.h"
#include "AnomalyDrive/ItemSystem/AnomaItem.h"
#include "AnomalyDrive/ItemSystem/AnomaItemCarPart.h"
#include "AnomalyDrive/Vehicle/VehicleBase.h"
#include "Engine/LocalPlayer.h"
#include "Components/BoxComponent.h"
#include "AnomalyDrive/Vehicle/VehicleBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

///---------------------------------------------------------------------------------------------------------------------
AAnomaPlayerCharacter::AAnomaPlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent
	{
		FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
		FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
		FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
		FirstPersonCameraComponent->bUsePawnControlRotation = true;
	}

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	{
		Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
		Mesh1P->SetOnlyOwnerSee(true);
		Mesh1P->SetupAttachment(FirstPersonCameraComponent);
		Mesh1P->bCastDynamicShadow = false;
		Mesh1P->CastShadow = false;
		Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	}

	// Create a mesh component for the item in hand
	{
		MeshItemInHand = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
		MeshItemInHand->SetupAttachment(GetCapsuleComponent());
		MeshItemInHand->bCastDynamicShadow = false;
		MeshItemInHand->CastShadow = false;
		MeshItemInHand->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::PutItemInHand(AAnomaItem* Item)
{
	ensureAlways(ItemInInventory[InventoryIndexInHand] == nullptr);

	const auto ItemDesc = Item->GetItemDesc();
	
	Item->OnPick(this);
	ItemInInventory[InventoryIndexInHand] = Item;
	MeshItemInHand->SetStaticMesh(ItemDesc.StaticMesh);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::DropItemInHand()
{
	if (ItemInInventory[InventoryIndexInHand] == nullptr)
		return;
	
	AAnomaItem* ItemInHand = ItemInInventory[InventoryIndexInHand];
	if(ItemInHand == nullptr)
		return;
		
	ItemInHand->OnDrop(this);
	ItemInInventory[InventoryIndexInHand] = nullptr;
	MeshItemInHand->SetStaticMesh(nullptr);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::UseVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation)
{
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::InstallVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation, AAnomaItemCarPart* CarPart)
{
	ensureAlways(Vehicle);
	ensureAlways(CarPartLocation != ECarPartLocation::None);
	
	Vehicle->InstallCarPart(CarPartLocation, CarPart);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ItemInInventory.AddDefaulted(5); // Hardcoded, 5 item in hand
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TickInteractionTrace(DeltaSeconds);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::BeginReplication()
{
	Super::BeginReplication();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::Move(const FVector2D& MovementVector)
{
	if (Controller == nullptr)
		return;
	
	AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	AddMovementInput(GetActorRightVector(), MovementVector.X);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::Look(const FVector2D& LookAxisVector)
{
	if (Controller == nullptr)
		return;
	
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::DropItem()
{
	DropItemInHand();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::Interact()
{
	if (HitResultInteraction.Num() == 0)
		return;

	AActor* ActorLookingAt = HitResultInteraction[0].GetActor();
	UPrimitiveComponent* ComponentLookingAt = HitResultInteraction[0].GetComponent();
	
	if (ActorLookingAt == nullptr)
		return;

	AAnomaItem* ItemInHand = ItemInInventory[InventoryIndexInHand];
	bool HasItemInHand = ItemInHand != nullptr;
	
	if (ActorLookingAt->ActorHasTag("Vehicle"))
	{
		const auto VehicleComponent = Cast<UBoxComponent>(ComponentLookingAt);
		check(VehicleComponent);
		
		AVehicleBase* Vehicle = Cast<AVehicleBase>(ActorLookingAt);

		const FName& Name = ComponentLookingAt->ComponentTags[0];
		const ECarPartLocation CarPartLocation = CarPartUtility::FNameToCarPartLocation(Name);

		const bool HasCarPartInstalled = Vehicle->HasInstalledCarPart(CarPartLocation);
		if (HasCarPartInstalled == false)
		{
			if (HasItemInHand == true)
			{
				return; // TODO Julien Rogel (05/05/2025)
			}
			else
			{
				UseVehicleCarPart(Vehicle, CarPartLocation);
			}
		}
	}
	else
	{
		if (HasItemInHand == true)
		{
			return; // TODO Julien Rogel (05/05/2025): 
		}
		else
		{
			AAnomaItem* ItemActor = Cast<AAnomaItem>(ActorLookingAt);
			PutItemInHand(ItemActor);
		}
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::StartVehicleModification()
{
	AActor* ActorLookingAt = HitResultInteraction[0].GetActor();
	UPrimitiveComponent* ComponentLookingAt = HitResultInteraction[0].GetComponent();
	
	if (ActorLookingAt == nullptr)
	{
		return;
	}

	AAnomaItem* ItemInHand = ItemInInventory[InventoryIndexInHand];
	bool HasItemInHand = ItemInHand != nullptr;
	
	if (ActorLookingAt->ActorHasTag("Vehicle") == false)
	{
		return;
	}
		
	const auto VehicleComponent = Cast<UBoxComponent>(ComponentLookingAt);
	check(VehicleComponent);
	
	VehicleAimedForModification = Cast<AVehicleBase>(ActorLookingAt);

	const FName& Name = ComponentLookingAt->ComponentTags[0];
	VehicleLocationAimedForModification = CarPartUtility::FNameToCarPartLocation(Name);
	ensureAlways(VehicleLocationAimedForModification != ECarPartLocation::None);

	const bool HasCarPartInstalled = VehicleAimedForModification->HasInstalledCarPart(VehicleLocationAimedForModification);

	if (HasCarPartInstalled == true && HasItemInHand == true)
		return; // TODO Julien Rogel (05/05/2025): To Improve 

	if (HasCarPartInstalled == false && HasItemInHand == false)
		return; // TODO Julien Rogel (05/05/2025): To Improve 
	
	check(IsModifyingVehicle == false);
	IsModifyingVehicle = true;
	if (HasCarPartInstalled == true)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandleVehicleModification, this, &AAnomaPlayerCharacter::UninstallCarPartCompleted,0.5f, false);
	}
	else
	{
		CarPartItemForModification = Cast<AAnomaItemCarPart>(ItemInHand);
		if (VehicleAimedForModification->CanInstallCarPart(VehicleLocationAimedForModification, CarPartItemForModification) == ECommonCarPartResult::Success)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandleVehicleModification, this, &AAnomaPlayerCharacter::InstallCarPartCompleted,0.5f, false);
		}
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::EndVehicleModification()
{
	if (IsModifyingVehicle == true)
	{
		CancelVehicleModification();
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::CancelVehicleModification()
{
	CleanVehicleModification();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::CleanVehicleModification()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleVehicleModification);
	IsModifyingVehicle = false;
	VehicleAimedForModification = nullptr;
	VehicleLocationAimedForModification = ECarPartLocation::None;
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::InstallCarPartCompleted()
{
	check(IsModifyingVehicle == true);

	InstallVehicleCarPart(VehicleAimedForModification, VehicleLocationAimedForModification, CarPartItemForModification);
	
	CleanVehicleModification();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::UninstallCarPartCompleted()
{
	ensureAlways(false);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::TickInteractionTrace(float DeltaSeconds)
{
	if (IsModifyingVehicle == true)
		return;
	
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * InteractionRange;
	const TArray<AActor*> ActorsToIgnore;
	HitResultInteraction.Empty();

	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, InteractionRadius, InteractionTraceQuery, false,
		ActorsToIgnore, EDrawDebugTrace::ForOneFrame,
		HitResultInteraction, true, FLinearColor::White, FLinearColor::Green, 0.0f);
}
///---------------------------------------------------------------------------------------------------------------------
