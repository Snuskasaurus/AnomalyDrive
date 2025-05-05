// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AnomaItem.generated.h"

class AAnomaPlayerCharacter;

USTRUCT(BlueprintType)
struct FItemDesc
{
	GENERATED_BODY()

	FString Name = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* StaticMesh = nullptr;
};

UCLASS(Blueprintable, Abstract)
class ANOMALYDRIVE_API AAnomaItem : public AActor
{
	GENERATED_BODY()

public:
	
	AAnomaItem();

protected:
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	const FItemDesc& GetItemDesc() const { return ItemDesc; }
	
public:
	
	void OnPick(AAnomaPlayerCharacter* Player);
	void OnDrop(AAnomaPlayerCharacter* Player);

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UBoxComponent* ColliderComponent;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) FItemDesc ItemDesc;
};
