// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "AnomaItem.h"
#include "AnomalyDrive/Player/AnomaPlayerCharacter.h"
#include "Components/BoxComponent.h"

///---------------------------------------------------------------------------------------------------------------------
AAnomaItem::AAnomaItem()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Mesh Component
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
		MeshComponent->SetCollisionProfileName("ItemPhysic");
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->SetEnableGravity(true);
		RootComponent = MeshComponent;
	}
	
	// Collider Component
	{
		ColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
		ColliderComponent->SetupAttachment(MeshComponent);
		ColliderComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
		ColliderComponent->SetCollisionProfileName(TEXT("Interactable"));
	}
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaItem::BeginPlay()
{
	Super::BeginPlay();
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaItem::OnPick(AAnomaPlayerCharacter* Player)
{
	MeshComponent->SetVisibility(false);
	MeshComponent->SetSimulatePhysics(false);
	this->SetReplicateMovement(false);
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	
	auto AttachmentTransformRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	AttachmentTransformRules.bWeldSimulatedBodies = true;
	const bool SuccessAttach = this->AttachToActor(Player, AttachmentTransformRules);
	check(SuccessAttach);
}
///---------------------------------------------------------------------------------------------------------------------
void AAnomaItem::OnDrop(AAnomaPlayerCharacter* Player)
{
	MeshComponent->SetVisibility(true);
	MeshComponent->SetSimulatePhysics(true);
	this->SetReplicateMovement(true);
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	
	this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}
///---------------------------------------------------------------------------------------------------------------------
