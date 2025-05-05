// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnomaItem.generated.h"

class AAnomaPlayerCharacter;

UCLASS()
class ANOMALYDRIVE_API AAnomaItem : public AActor
{
	GENERATED_BODY()

public:
	AAnomaItem();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
