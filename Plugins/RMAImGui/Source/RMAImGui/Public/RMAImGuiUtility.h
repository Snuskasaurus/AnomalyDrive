// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#pragma once
#include "RMAImGuiDef.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RMAImGuiUtility.generated.h"

class URMAImGuiHook;

//Function Library
UCLASS()
class RMAIMGUI_API URMAImGuiFunctionLibrary : public UBlueprintFunctionLibrary
{

    GENERATED_BODY()

public:

    //Getter Plugin Version
    UFUNCTION(BlueprintPure, Category = "RMAImGui")
        static FName GetVersion();

    //Getter ImGui Version
    UFUNCTION(BlueprintPure, Category = "RMAImGui")
        static FName GetVersionImGui();

    //Getter ImPlot Version
    UFUNCTION(BlueprintPure, Category = "RMAImGui")
        static FName GetVersionImPlot();

    //Getter ImSpinner Version
    UFUNCTION(BlueprintPure, Category = "RMAImGui")
        static FName GetVersionImSpinner();

    //Add Hook
    UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
        static bool AddHook(URMAImGuiHook* InHook, const FName InContext = "Default");

    //Remove Hook
    UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
        static bool RemoveHook(URMAImGuiHook* InHook, const FName InContext = "Default");

    //Flush Hooks
    UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
        static bool FlushHooks(const FName InContext = "Default");

};
