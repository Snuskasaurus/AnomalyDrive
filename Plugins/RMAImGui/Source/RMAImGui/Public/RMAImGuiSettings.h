// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#pragma once
#include "Engine/PlatformSettings.h"
#include "RMAImGuiSettings.generated.h"

//Input Key Pair
USTRUCT()
struct RMAIMGUI_API FRMAImGuiInputKeyPair
{

	GENERATED_BODY()

public:

	//Key A
	UPROPERTY(EditDefaultsOnly, Category = "Keys")
		FKey KeyA;

	//Key B
	UPROPERTY(EditDefaultsOnly, Category = "Keys")
		FKey KeyB;

public:

	FRMAImGuiInputKeyPair(FKey InKeyA = EKeys::Invalid, FKey InKeyB = EKeys::Invalid)
	{
		KeyA = InKeyA;
		KeyB = InKeyB;
	};

	//Contains
	bool Contains(FKey InKey) const { return (KeyA.IsValid() && InKey == KeyA) || (KeyB.IsValid() && InKey == KeyB); }

	//Match
	bool Match(TArray<FKey> InKeys) const
	{
		if (KeyA.IsValid() || KeyB.IsValid())
		{
			return (!KeyA.IsValid() || InKeys.Contains(KeyA)) && (!KeyB.IsValid() || InKeys.Contains(KeyB));
		}
		return false;
	}

};

//Settings
UCLASS(Config = Engine, DefaultConfig, Meta = (DisplayName = "RMA ImGui"))
class RMAIMGUI_API URMAImGuiSettings : public UDeveloperSettings
{

	GENERATED_BODY()

public:

	URMAImGuiSettings(const FObjectInitializer& InInitializer) : Super(InInitializer) {};

public:

	//Font
	UPROPERTY(Config, EditDefaultsOnly, Category = "Fonts", meta = (AllowedClasses = "Font", ConfigRestartRequired = true))
		FSoftObjectPath Font = FSoftObjectPath("/RMAImGui/Fonts/F_NotoSans.F_NotoSans");

	//Font Size
	UPROPERTY(Config, EditDefaultsOnly, Category = "Fonts", meta = (ConfigRestartRequired = true))
		int32 FontSize = 16;

	//Alpha When Focused
	UPROPERTY(Config, EditDefaultsOnly, Category = "Style")
		float AlphaWhenFocused = 1.f;

	//Alpha When Not Focused
	UPROPERTY(Config, EditDefaultsOnly, Category = "Style")
		float AlphaWhenNotFocused = 0.5f;

	//Blur Strength
	UPROPERTY(Config, EditDefaultsOnly, Category = "Style")
		float BlurStrength = 1.f;

	//Toggle Demos
	UPROPERTY(Config, EditDefaultsOnly, Category = "Inputs")
		TArray<FRMAImGuiInputKeyPair> ToggleDemos = { FRMAImGuiInputKeyPair(EKeys::PageUp), FRMAImGuiInputKeyPair(EKeys::Gamepad_Special_Left, EKeys::Gamepad_DPad_Right) };

	//Toggle Global Visibility
	UPROPERTY(Config, EditDefaultsOnly, Category = "Inputs")
		TArray<FRMAImGuiInputKeyPair> ToggleVisibility = { FRMAImGuiInputKeyPair(EKeys::Home), FRMAImGuiInputKeyPair(EKeys::Gamepad_Special_Left, EKeys::Gamepad_DPad_Down) };

	//Toggle Global Controls
	UPROPERTY(Config, EditDefaultsOnly, Category = "Inputs")
		TArray<FRMAImGuiInputKeyPair> ToggleControls = { FRMAImGuiInputKeyPair(EKeys::Insert), FRMAImGuiInputKeyPair(EKeys::Gamepad_Special_Left, EKeys::Gamepad_DPad_Up) };

	//Default Global Visibility
	UPROPERTY(Config, EditDefaultsOnly, Category = "Defaults")
		bool DefaultVisibility = true;

	//Default Global Controls
	UPROPERTY(Config, EditDefaultsOnly, Category = "Defaults")
		bool DefaultControls = false;

	//Draw Cursor
	UPROPERTY(Config, EditDefaultsOnly, Category = "Cursor")
		bool DrawCursor = true;

	//Include Demos
	UPROPERTY(Config, EditDefaultsOnly, Category = "Include")
		bool IncludeDemos = true;

	//Development Only
	UPROPERTY(Config, EditDefaultsOnly, Category = "Development")
		bool DevelopmentOnly = true;

};
