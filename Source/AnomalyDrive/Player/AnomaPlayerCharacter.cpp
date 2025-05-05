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
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::PickUpItem(AAnomaItem* Item)
{
	
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::DropItemInHand()
{
	
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::InteractWithVehicleCarPart(AVehicleBase* Vehicle, UBoxComponent* CarPartCollider)
{
	if (!ensureAlways(CarPartCollider->ComponentTags.IsEmpty() == false))
		return;
	
	const FName& Name = CarPartCollider->ComponentTags[0];
	const ECarPartLocation CarPartLocation = CarPartUtility::FNameToCarPartLocation(Name);

	const bool HasCarPartInstalled = Vehicle->HasInstalledCarPart(CarPartLocation);
	if (HasCarPartInstalled == false)
	{
		UseVehicleCarPart(Vehicle, CarPartLocation);
	}
	else
	{
		InstallVehicleCarPart(Vehicle, CarPartLocation);
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::UseVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation)
{
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::InstallVehicleCarPart(AVehicleBase* Vehicle, const ECarPartLocation CarPartLocation)
{
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
void AAnomaPlayerCharacter::Interact()
{
	if (HitResultInteraction.Num() == 0)
		return;

	AActor* ActorLookingAt = HitResultInteraction[0].GetActor();
	UPrimitiveComponent* ComponentLookingAt = HitResultInteraction[0].GetComponent();
	
	if (ActorLookingAt == nullptr)
		return;
		
	if (ActorLookingAt->ActorHasTag("Vehicle"))
	{
		const auto VehicleComponent = Cast<UBoxComponent>(ComponentLookingAt);
		check(VehicleComponent);
		
		AVehicleBase* Vehicle = Cast<AVehicleBase>(ActorLookingAt);
		InteractWithVehicleCarPart(Vehicle, VehicleComponent);
	}
	else
	{
		
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaPlayerCharacter::TickInteractionTrace(float DeltaSeconds)
{
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector End = Start + FirstPersonCameraComponent->GetForwardVector() * InteractionRange;
	const TArray<AActor*> ActorsToIgnore;
	HitResultInteraction.Empty();

	UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, InteractionRadius, InteractionTraceQuery, false,
		ActorsToIgnore, EDrawDebugTrace::ForOneFrame, HitResultInteraction, true, FLinearColor::White, FLinearColor::Green, 0.0f);
}
///---------------------------------------------------------------------------------------------------------------------
