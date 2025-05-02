// Copyright (c) 2025 Julien Rogel. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VehicleSystemBase.h"
#include "VehicleBase.generated.h"

UCLASS(Blueprintable, Abstract)
class ANOMALYDRIVE_API AVehicleBase : public AVehicleSystemBase
{
	GENERATED_BODY()
public:
	AVehicleBase();
};
