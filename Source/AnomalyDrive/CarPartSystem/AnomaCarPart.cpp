// Copyright (c) 2025 Julien Rogel. All rights reserved.

#include "AnomaCarPart.h"

AAnomaCarPart::AAnomaCarPart()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAnomaCarPart::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAnomaCarPart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

