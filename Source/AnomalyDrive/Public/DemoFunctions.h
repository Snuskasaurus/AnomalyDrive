#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DemoFunctions.generated.h"

UCLASS()
class UDemoFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//Returns the project version set in the 'Project Settings' > 'Description' section of the editor
	UFUNCTION(BlueprintPure, Category = "Project")
	static FString GetProjectVersion();
	
};
