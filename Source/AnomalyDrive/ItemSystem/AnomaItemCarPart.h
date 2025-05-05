// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnomaItem.h"
#include "AnomalyDrive/CarPartSystem/CarPartSystem.h"
#include "AnomalyDrive/ItemSystem/AnomaItem.h"
#include "GameFramework/Actor.h"
#include "AnomaItemCarPart.generated.h"

USTRUCT(BlueprintType)
struct FItemCarPartDesc
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) ECarPartType CarPartType = ECarPartType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* StaticMesh = nullptr;
};

UCLASS(BlueprintType, Abstract)
class AAnomaItemCarPart : public AAnomaItem
{
	GENERATED_BODY()

public:
	AAnomaItemCarPart();

public:
	const FItemCarPartDesc& GetItemCarPartDesc() const { return ItemCarPartDesc; };
	const FCarPartStatus& GetCarPartStatus() const { return CarPartStatus; };
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FItemCarPartDesc ItemCarPartDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCarPartStatus CarPartStatus;
};
