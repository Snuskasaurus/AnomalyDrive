// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#include "RMAImGuiSlate.h"
#include "RMAImGuiSettings.h"
#include "RMAImGuiHook.h"
#include "Slate/SlateTextures.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Engine/Texture2D.h"
#include "Engine/GameViewportClient.h"
#include "Misc/FileHelper.h"
#include "Interfaces/IPluginManager.h"
#include "Brushes/SlateDynamicImageBrush.h"
#include "Framework/Application/SlateUser.h"

#if STATS
DEFINE_STAT(STAT_HOOKS);
DEFINE_STAT(STAT_INPUTS);
DEFINE_STAT(STAT_RENDER);
DEFINE_STAT(STAT_BATCHES);
DEFINE_STAT(STAT_VERTICES);
DEFINE_STAT(STAT_INDICES);
DEFINE_STAT(STAT_TRIANGLES);
DEFINE_STAT(STAT_RESOURCES);
DEFINE_STAT(STAT_RESOURCES_MEMORY);
DEFINE_STAT(STAT_RESOURCES_IN_USE_MEMORY);
#endif

namespace RMAImGui
{
	static URMAImGuiSettings* GSettings = nullptr;
	namespace Font
	{

		struct FIndex;

		static TArray<FIndex> GIndexes = {};
		static UFont* GFont = nullptr;
		static ImFontAtlas GAtlas = ImFontAtlas();

		struct FIndex
		{

		private:

			FString Name = "";
			bool Default = false;
			TArray<uint8> Data = {};
			ImVector<ImWchar> Ranges;

		public:

			FIndex() = delete;
			FIndex(FString InName, bool InDefault, FTypeface InTypeface, TArray<FInt32Range> InRanges = {})
			{
				Name = InName;
				Default = InDefault;
				if (InTypeface.Fonts.IsValidIndex(0))
				{
					const auto LSrc = &InTypeface.Fonts[0].Font.GetFontFaceData()->GetData();
					if (LSrc)
					{
						Data.Empty();
						Data.AddUninitialized(LSrc->Num());
						FMemory::Memcpy(Data.GetData(), LSrc->GetData(), LSrc->GetAllocatedSize());
					}
				}
				for (const auto LRange : InRanges)
				{
					Ranges.push_back(LRange.GetLowerBoundValue());
					Ranges.push_back(LRange.GetUpperBoundValue());
				}
				if (!Ranges.empty()) { Ranges.push_back(0); }
			}

			bool IsValid() const { return !Name.IsEmpty() && Data.Num() > 0; }
			FString GetName() const { return Name; }
			TArray<uint8> GetData() const { return Data; }

			const ImWchar* GetRanges() const
			{
				if (GSettings)
				{
					if (Default) { return GAtlas.GetGlyphRangesDefault(); }
					else if (!Ranges.empty()) { return &Ranges.front(); }
					else if (Name.Contains("JP")) { return GAtlas.GetGlyphRangesJapanese(); }
					else if (Name.Contains("KR")) { return GAtlas.GetGlyphRangesKorean(); }
					else if (Name.Contains("SC")) { return GAtlas.GetGlyphRangesChineseSimplifiedCommon(); }
					else if (Name.Contains("TC")) { return GAtlas.GetGlyphRangesChineseFull(); }
				}
				return nullptr;
			}

			ImFontConfig* GetConfig() const
			{
				auto LConfig = new ImFontConfig();
				LConfig->MergeMode = !Default;
				LConfig->FontDataOwnedByAtlas = false;
				TCString<char>::Snprintf(LConfig->Name, IM_ARRAYSIZE(LConfig->Name), "%s", TCHAR_TO_ANSI(*Name));
				return LConfig;
			}

		};

		bool IsValid() { return GFont != nullptr; }

		bool IsBuilt() { return IsValid() && GAtlas.IsBuilt(); }

		bool Load()
		{
			if (GSettings && !IsValid())
			{
				GIndexes.Empty();
				GFont = Cast<UFont>(GSettings->Font.TryLoad());
				const FCompositeFont* LComposite = GFont ? GFont->GetCompositeFont() : nullptr;
				if (LComposite)
				{
					GIndexes.Add(FIndex(GFont->GetName(), true, LComposite->DefaultTypeface));
					for (const auto LSub : LComposite->SubTypefaces)
					{ GIndexes.Add(FIndex((GFont->GetName() + LSub.Cultures), false, LSub.Typeface, LSub.CharacterRanges)); }
				}
			}
			return IsValid();
		}

		bool Build()
		{
			if (GSettings && !IsBuilt() && GIndexes.Num() > 0)
			{
				for (const auto LIndex : GIndexes)
				{
					GAtlas.AddFontFromMemoryTTF((void*)LIndex.GetData().GetData(), LIndex.GetData().GetAllocatedSize(), GSettings->FontSize, LIndex.GetConfig(), LIndex.GetRanges());
				}
				if (GAtlas.Build()) { GAtlas.TexID = "Font"; }
			}
			return IsBuilt();
		}

		bool AddResource(SRMAImGuiContext* InContext)
		{
			if (InContext && GAtlas.IsBuilt())
			{
				GAtlas.GetTexDataAsRGBA32(new uint8*, nullptr, nullptr);
				return InContext->AddResource(
					"Font",
					TArray<uint8>((uint8*)GAtlas.TexPixelsRGBA32, (GAtlas.TexWidth * GAtlas.TexHeight * 4)),
					FVector2D(GAtlas.TexWidth, GAtlas.TexHeight));
			}
			return false;
		}

		void Reset()
		{
			GFont = nullptr;
			GIndexes.Empty();
			GAtlas = ImFontAtlas();
		}

	}

	bool LoadSettings()
	{
		GSettings = GetMutableDefault<URMAImGuiSettings>();
		if (GSettings)
		{
			if (!Font::Load()) { UE_LOG(LOG_RMAIMGUI, Warning, TEXT("Failed To Load Fonts")); return false; }
			UE_LOG(LOG_RMAIMGUI, Display, TEXT("Loaded Settings"));
			return true;
		}
		UE_LOG(LOG_RMAIMGUI, Warning, TEXT("Failed To Load Settings"));
		return false;
	}

	void ResetSettings()
	{
		GSettings = nullptr;
		Font::Reset();
		UE_LOG(LOG_RMAIMGUI, Display, TEXT("Reset Settings"));
	}

}

bool SRMAImGuiContext::FInputRouter::RouteKey(const FKey InKey, const EInputEvent InEvent, const bool InUppercase)
{
	const FKey LSrcKey = InKey;
	const FKey LRepKey = ReplaceKey(LSrcKey);
	if (CanRoute(LRepKey))
	{
		const int LAPIChar = GetAPIChar(LRepKey);
		const int LAPIModifier = GetAPIModifier(LRepKey);
		const int LAPIKey = (LAPIModifier >= 0) ? LAPIModifier : GetAPIKey(LRepKey);
		if (LAPIKey >= 0)
		{
			StackIn(LRepKey, InEvent, true);//(IN) API
			if (LRepKey.IsMouseButton())
			{
				Context->GetIOImGui()->AddMouseButtonEvent(LAPIKey, (InEvent == IE_Pressed) || (InEvent == IE_Repeat) || (InEvent == IE_DoubleClick));
				return !StackOut(LSrcKey, InEvent, false);//(OUT) Engine
			}
			if (LAPIModifier >= 0)
			{
				Context->GetIOImGui()->AddKeyEvent(ImGuiKey(LAPIModifier), (InEvent == IE_Pressed) || (InEvent == IE_Repeat) || (InEvent == IE_DoubleClick));
			}
			else
			{
				if (!IsModifierPressed() && Context->GetIOImGui()->WantTextInput && (LAPIChar >= 0) 
					&& (InEvent == IE_Pressed || InEvent == IE_Repeat || InEvent == IE_DoubleClick))
				{
					Context->GetIOImGui()->AddInputCharacter(InUppercase ? FChar::ToUpper(LAPIChar) : FChar::ToLower(LAPIChar));
				}
				else
				{
					Context->GetIOImGui()->AddKeyEvent(ImGuiKey(LAPIKey), (InEvent == IE_Pressed) || (InEvent == IE_Repeat) || (InEvent == IE_DoubleClick));
				}
			}
			return !StackOut(LSrcKey, InEvent, false);//(OUT) Engine
		}
	}
	StackIn(LSrcKey, InEvent, false);//(IN) Engine
	StackOut(LRepKey, InEvent, true);//(OUT) API
	return false;
}

bool SRMAImGuiContext::FInputRouter::RouteAxis(const FKey InKey, const float InDelta)
{
	if (CanRoute(InKey) && FMath::Abs(InDelta) > 0)
	{
		const int LAPIKey = GetAPIAxis(InKey);
		if (LAPIKey >= 0)
		{
			if (InKey.IsMouseButton())
			{
				Context->GetIOImGui()->AddMouseWheelEvent(0, InDelta);
				return true;
			}
		}
	}
	return false;
}

bool SRMAImGuiContext::FInputRouter::RouteCursor(const FVector2D InDelta, TSharedPtr<ICursor> InCursor)
{
	if (Context && Context->GetIOImGui())
	{
		if (CanRoute((Context->GetType() == RMAImGui::EType::Editor) ? EKeys::Mouse2D : EKeys::AnyKey))
		{
			if (FSlateApplication::Get().HasAnyMouseCaptor())
			{
				if (Context->GetType() == RMAImGui::EType::Gameplay)
				{
					if (!CaptorWidget.IsValid() && FSlateApplication::Get().GetCursorUser().IsValid())
					{
						CaptorWidget = FSlateApplication::Get().GetCursorUser()->GetFocusedWidget();
						if (CaptorWidget.IsValid()) { FSlateApplication::Get().GetCursorUser()->ReleaseCursorCapture(); }
					}
				}
				else { return false; }
			}
			if (!FSlateApplication::Get().HasAnyMouseCaptor())
			{
				const FVector2D LPosition = Context->GetTickSpaceGeometry().AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
				Context->GetIOImGui()->AddMousePosEvent(LPosition.X, LPosition.Y);
				return true;
			}
		}
	}
	RestoreCaptor();
	return false;
}

void SRMAImGuiContext::FInputRouter::StackIn(const FKey InKey, const EInputEvent InEvent, const bool InAPI)
{
	if (InKey.IsValid() && InEvent == EInputEvent::IE_Pressed || InEvent == EInputEvent::IE_Repeat || InEvent == EInputEvent::IE_DoubleClick)
	{
		(InAPI ? StackAPI : StackEngine).Add(InKey, InEvent);
	}
}

bool SRMAImGuiContext::FInputRouter::StackOut(const FKey InKey, const EInputEvent InEvent, const bool InAPI)
{
	if (InKey.IsValid() && (InEvent == EInputEvent::IE_Released || InAPI))
	{
		if ((InAPI ? StackAPI : StackEngine).Contains(InKey))
		{
			(InAPI ? StackAPI : StackEngine).Remove(InKey);
			if (InAPI)
			{
				if (InKey.IsMouseButton()) { Context->GetIOImGui()->AddMouseButtonEvent(GetAPIKey(InKey), false); }
				Context->GetIOImGui()->AddKeyEvent(ImGuiKey(GetAPIKey(InKey)), false);
			}
			return true;
		}
	}
	return false;
}

FKey SRMAImGuiContext::FInputRouter::ReplaceKey(const FKey InKey) const
{
	//e.g
	//if (InKey == EKeys::NumPadZero) { return EKeys::Zero; }
	return InKey;
}

int SRMAImGuiContext::FInputRouter::GetAPIKey(const FKey InKey) const
{
	if (InKey == EKeys::LeftMouseButton) return ImGuiMouseButton_::ImGuiMouseButton_Left;
	if (InKey == EKeys::RightMouseButton) return ImGuiMouseButton_::ImGuiMouseButton_Right;
	if (InKey == EKeys::MiddleMouseButton) return ImGuiMouseButton_::ImGuiMouseButton_Middle;
	if (InKey == EKeys::BackSpace) return ImGuiKey::ImGuiKey_Backspace;
	if (InKey == EKeys::Tab) return ImGuiKey::ImGuiKey_Tab;
	if (InKey == EKeys::Enter) return ImGuiKey::ImGuiKey_Enter;
	if (InKey == EKeys::Pause) return ImGuiKey::ImGuiKey_Pause;
	if (InKey == EKeys::CapsLock) return ImGuiKey::ImGuiKey_CapsLock;
	if (InKey == EKeys::Escape) return ImGuiKey::ImGuiKey_Escape;
	if (InKey == EKeys::SpaceBar) return ImGuiKey::ImGuiKey_Space;
	if (InKey == EKeys::PageUp) return ImGuiKey::ImGuiKey_PageUp;
	if (InKey == EKeys::PageDown) return ImGuiKey::ImGuiKey_PageDown;
	if (InKey == EKeys::End) return ImGuiKey::ImGuiKey_End;
	if (InKey == EKeys::Home) return ImGuiKey::ImGuiKey_Home;
	if (InKey == EKeys::Left) return ImGuiKey::ImGuiKey_LeftArrow;
	if (InKey == EKeys::Up) return ImGuiKey::ImGuiKey_UpArrow;
	if (InKey == EKeys::Right) return ImGuiKey::ImGuiKey_RightArrow;
	if (InKey == EKeys::Down) return ImGuiKey::ImGuiKey_DownArrow;
	if (InKey == EKeys::Insert) return ImGuiKey::ImGuiKey_Insert;
	if (InKey == EKeys::Delete) return ImGuiKey::ImGuiKey_Delete;
	if (InKey == EKeys::Zero) return ImGuiKey::ImGuiKey_0;
	if (InKey == EKeys::One) return ImGuiKey::ImGuiKey_1;
	if (InKey == EKeys::Two) return ImGuiKey::ImGuiKey_2;
	if (InKey == EKeys::Three) return ImGuiKey::ImGuiKey_3;
	if (InKey == EKeys::Four) return ImGuiKey::ImGuiKey_4;
	if (InKey == EKeys::Five) return ImGuiKey::ImGuiKey_5;
	if (InKey == EKeys::Six) return ImGuiKey::ImGuiKey_6;
	if (InKey == EKeys::Seven) return ImGuiKey::ImGuiKey_7;
	if (InKey == EKeys::Eight) return ImGuiKey::ImGuiKey_8;
	if (InKey == EKeys::Nine) return ImGuiKey::ImGuiKey_9;
	if (InKey == EKeys::A) return ImGuiKey::ImGuiKey_A;
	if (InKey == EKeys::B) return ImGuiKey::ImGuiKey_B;
	if (InKey == EKeys::C) return ImGuiKey::ImGuiKey_C;
	if (InKey == EKeys::D) return ImGuiKey::ImGuiKey_D;
	if (InKey == EKeys::E) return ImGuiKey::ImGuiKey_E;
	if (InKey == EKeys::F) return ImGuiKey::ImGuiKey_F;
	if (InKey == EKeys::G) return ImGuiKey::ImGuiKey_G;
	if (InKey == EKeys::H) return ImGuiKey::ImGuiKey_H;
	if (InKey == EKeys::I) return ImGuiKey::ImGuiKey_I;
	if (InKey == EKeys::J) return ImGuiKey::ImGuiKey_J;
	if (InKey == EKeys::K) return ImGuiKey::ImGuiKey_K;
	if (InKey == EKeys::L) return ImGuiKey::ImGuiKey_L;
	if (InKey == EKeys::M) return ImGuiKey::ImGuiKey_M;
	if (InKey == EKeys::N) return ImGuiKey::ImGuiKey_N;
	if (InKey == EKeys::O) return ImGuiKey::ImGuiKey_O;
	if (InKey == EKeys::P) return ImGuiKey::ImGuiKey_P;
	if (InKey == EKeys::Q) return ImGuiKey::ImGuiKey_Q;
	if (InKey == EKeys::R) return ImGuiKey::ImGuiKey_R;
	if (InKey == EKeys::S) return ImGuiKey::ImGuiKey_S;
	if (InKey == EKeys::T) return ImGuiKey::ImGuiKey_T;
	if (InKey == EKeys::U) return ImGuiKey::ImGuiKey_U;
	if (InKey == EKeys::V) return ImGuiKey::ImGuiKey_V;
	if (InKey == EKeys::W) return ImGuiKey::ImGuiKey_W;
	if (InKey == EKeys::X) return ImGuiKey::ImGuiKey_X;
	if (InKey == EKeys::Y) return ImGuiKey::ImGuiKey_Y;
	if (InKey == EKeys::Z) return ImGuiKey::ImGuiKey_Z;
	if (InKey == EKeys::NumPadZero) return ImGuiKey::ImGuiKey_Keypad0;
	if (InKey == EKeys::NumPadOne) return ImGuiKey::ImGuiKey_Keypad1;
	if (InKey == EKeys::NumPadTwo) return ImGuiKey::ImGuiKey_Keypad2;
	if (InKey == EKeys::NumPadThree) return ImGuiKey::ImGuiKey_Keypad3;
	if (InKey == EKeys::NumPadFour) return ImGuiKey::ImGuiKey_Keypad4;
	if (InKey == EKeys::NumPadFive) return ImGuiKey::ImGuiKey_Keypad5;
	if (InKey == EKeys::NumPadSix) return ImGuiKey::ImGuiKey_Keypad6;
	if (InKey == EKeys::NumPadSeven) return ImGuiKey::ImGuiKey_Keypad7;
	if (InKey == EKeys::NumPadEight) return ImGuiKey::ImGuiKey_Keypad8;
	if (InKey == EKeys::NumPadNine) return ImGuiKey::ImGuiKey_Keypad9;
	if (InKey == EKeys::Multiply) return ImGuiKey::ImGuiKey_KeypadMultiply;
	if (InKey == EKeys::Add) return ImGuiKey::ImGuiKey_KeypadAdd;
	if (InKey == EKeys::Subtract) return ImGuiKey::ImGuiKey_KeypadSubtract;
	if (InKey == EKeys::Decimal) return ImGuiKey::ImGuiKey_KeypadDecimal;
	if (InKey == EKeys::Divide) return ImGuiKey::ImGuiKey_KeypadDivide;
	if (InKey == EKeys::F1) return ImGuiKey::ImGuiKey_F1;
	if (InKey == EKeys::F2) return ImGuiKey::ImGuiKey_F2;
	if (InKey == EKeys::F3) return ImGuiKey::ImGuiKey_F3;
	if (InKey == EKeys::F4) return ImGuiKey::ImGuiKey_F4;
	if (InKey == EKeys::F5) return ImGuiKey::ImGuiKey_F5;
	if (InKey == EKeys::F6) return ImGuiKey::ImGuiKey_F6;
	if (InKey == EKeys::F7) return ImGuiKey::ImGuiKey_F7;
	if (InKey == EKeys::F8) return ImGuiKey::ImGuiKey_F8;
	if (InKey == EKeys::F9) return ImGuiKey::ImGuiKey_F9;
	if (InKey == EKeys::F10) return ImGuiKey::ImGuiKey_F10;
	if (InKey == EKeys::F11) return ImGuiKey::ImGuiKey_F11;
	if (InKey == EKeys::F12) return ImGuiKey::ImGuiKey_F12;
	if (InKey == EKeys::NumLock) return ImGuiKey::ImGuiKey_NumLock;
	if (InKey == EKeys::ScrollLock) return ImGuiKey::ImGuiKey_ScrollLock;
	if (InKey == EKeys::LeftShift) return ImGuiKey::ImGuiKey_LeftShift;
	if (InKey == EKeys::RightShift) return ImGuiKey::ImGuiKey_RightShift;
	if (InKey == EKeys::LeftControl) return ImGuiKey::ImGuiKey_LeftCtrl;
	if (InKey == EKeys::RightControl) return ImGuiKey::ImGuiKey_RightCtrl;
	if (InKey == EKeys::LeftAlt) return ImGuiKey::ImGuiKey_LeftAlt;
	if (InKey == EKeys::RightAlt) return ImGuiKey::ImGuiKey_RightAlt;
	if (InKey == EKeys::Semicolon) return ImGuiKey::ImGuiKey_Semicolon;
	if (InKey == EKeys::Equals) return ImGuiKey::ImGuiKey_Equal;
	if (InKey == EKeys::Comma) return ImGuiKey::ImGuiKey_Comma;
	if (InKey == EKeys::Period) return ImGuiKey::ImGuiKey_Period;
	if (InKey == EKeys::Slash) return ImGuiKey::ImGuiKey_Slash;
	if (InKey == EKeys::LeftBracket) return ImGuiKey::ImGuiKey_LeftBracket;
	if (InKey == EKeys::Backslash) return ImGuiKey::ImGuiKey_Backslash;
	if (InKey == EKeys::RightBracket) return ImGuiKey::ImGuiKey_RightBracket;
	if (InKey == EKeys::Apostrophe) return ImGuiKey::ImGuiKey_Apostrophe;
	if (InKey == EKeys::Platform_Delete) return ImGuiKey::ImGuiKey_Delete;
	if (InKey == EKeys::Gamepad_LeftThumbstick) return ImGuiKey::ImGuiKey_GamepadL3;
	if (InKey == EKeys::Gamepad_RightThumbstick) return ImGuiKey::ImGuiKey_GamepadR3;
	if (InKey == EKeys::Gamepad_Special_Left) return ImGuiKey::ImGuiKey_GamepadBack;
	if (InKey == EKeys::Gamepad_Special_Right) return ImGuiKey::ImGuiKey_GamepadStart;
	if (InKey == EKeys::Gamepad_FaceButton_Bottom) return ImGuiKey::ImGuiKey_GamepadFaceDown;
	if (InKey == EKeys::Gamepad_FaceButton_Right) return ImGuiKey::ImGuiKey_GamepadFaceRight;
	if (InKey == EKeys::Gamepad_FaceButton_Left) return ImGuiKey::ImGuiKey_GamepadFaceLeft;
	if (InKey == EKeys::Gamepad_FaceButton_Top) return ImGuiKey::ImGuiKey_GamepadFaceUp;
	if (InKey == EKeys::Gamepad_LeftShoulder) return ImGuiKey::ImGuiKey_GamepadL1;
	if (InKey == EKeys::Gamepad_RightShoulder) return ImGuiKey::ImGuiKey_GamepadR1;
	if (InKey == EKeys::Gamepad_LeftTrigger) return ImGuiKey::ImGuiKey_GamepadL2;
	if (InKey == EKeys::Gamepad_RightTrigger) return ImGuiKey::ImGuiKey_GamepadR2;
	if (InKey == EKeys::Gamepad_DPad_Up) return ImGuiKey::ImGuiKey_GamepadDpadUp;
	if (InKey == EKeys::Gamepad_DPad_Down) return ImGuiKey::ImGuiKey_GamepadDpadDown;
	if (InKey == EKeys::Gamepad_DPad_Right) return ImGuiKey::ImGuiKey_GamepadDpadRight;
	if (InKey == EKeys::Gamepad_DPad_Left) return ImGuiKey::ImGuiKey_GamepadDpadLeft;
	if (InKey == EKeys::Gamepad_LeftStick_Up) return ImGuiKey::ImGuiKey_GamepadLStickUp;
	if (InKey == EKeys::Gamepad_LeftStick_Down) return ImGuiKey::ImGuiKey_GamepadLStickDown;
	if (InKey == EKeys::Gamepad_LeftStick_Right) return ImGuiKey::ImGuiKey_GamepadLStickRight;
	if (InKey == EKeys::Gamepad_LeftStick_Left) return ImGuiKey::ImGuiKey_GamepadLStickLeft;
	if (InKey == EKeys::Gamepad_RightStick_Up) return ImGuiKey::ImGuiKey_GamepadRStickUp;
	if (InKey == EKeys::Gamepad_RightStick_Down) return ImGuiKey::ImGuiKey_GamepadRStickDown;
	if (InKey == EKeys::Gamepad_RightStick_Right) return ImGuiKey::ImGuiKey_GamepadRStickRight;
	if (InKey == EKeys::Gamepad_RightStick_Left) return ImGuiKey::ImGuiKey_GamepadRStickLeft;
	return -1;
}

int SRMAImGuiContext::FInputRouter::GetAPIAxis(const FKey InKey) const
{
	if (InKey == EKeys::MouseWheelAxis) return ImGuiMouseButton_::ImGuiMouseButton_Middle;
	return -1;
}

int SRMAImGuiContext::FInputRouter::GetAPIChar(const FKey InKey) const
{
	//First Try To Get From KeyManager
	{
		const uint32* LKeyCode;
		const uint32* LCharCode;
		FInputKeyManager::Get().GetCodesFromKey(InKey, LKeyCode, LCharCode);
		if (LCharCode) { return *LCharCode; }
	}
	{
		if (InKey == EKeys::NumPadZero) { return '0'; }
		if (InKey == EKeys::NumPadOne) { return '1'; }
		if (InKey == EKeys::NumPadTwo) { return '2'; }
		if (InKey == EKeys::NumPadThree) { return '3'; }
		if (InKey == EKeys::NumPadFour) { return '4'; }
		if (InKey == EKeys::NumPadFive) { return '5'; }
		if (InKey == EKeys::NumPadSix) { return '6'; }
		if (InKey == EKeys::NumPadSeven) { return '7'; }
		if (InKey == EKeys::NumPadEight) { return '8'; }
		if (InKey == EKeys::NumPadNine) { return '9'; }
		if (InKey == EKeys::Multiply) { return '*'; }
		if (InKey == EKeys::Add) { return '+'; }
		if (InKey == EKeys::Subtract) { return '-'; }
		if (InKey == EKeys::Decimal) { return ','; }
		if (InKey == EKeys::Divide) { return '/'; }
	}
	return -1;
}

int SRMAImGuiContext::FInputRouter::GetAPIModifier(const FKey InKey) const
{
#if PLATFORM_WINDOWS
	if (InKey == EKeys::LeftControl) return ImGuiKey::ImGuiMod_Shortcut;
	if (InKey == EKeys::RightControl) return ImGuiKey::ImGuiMod_Shortcut;
#endif
#if PLATFORM_MAC
	if (InKey == EKeys::LeftCommand) return ImGuiKey::ImGuiMod_Shortcut;
	if (InKey == EKeys::RightCommand) return ImGuiKey::ImGuiMod_Shortcut;
#endif
	if (InKey == EKeys::LeftShift) return ImGuiKey::ImGuiMod_Shift;
	if (InKey == EKeys::RightShift) return ImGuiKey::ImGuiMod_Shift;
	if (InKey == EKeys::LeftAlt) return ImGuiKey::ImGuiMod_Alt;
	if (InKey == EKeys::RightAlt) return ImGuiKey::ImGuiMod_Alt;
	return -1;
}

bool SRMAImGuiContext::FInputRouter::CanRoute(const FKey InKey) const
{
	return Context && Context->CanRouteInputs(InKey);
}

bool SRMAImGuiContext::FInputRouter::IsModifierPressed() const
{
	return ImGui::IsKeyDown(ImGuiMod_Ctrl) || ImGui::IsKeyDown(ImGuiMod_Shift) || ImGui::IsKeyDown(ImGuiMod_Alt) || ImGui::IsKeyDown(ImGuiMod_Super) || ImGui::IsKeyDown(ImGuiMod_Shortcut);
}

void SRMAImGuiContext::FInputRouter::RestoreCaptor()
{
	if (CaptorWidget.IsValid())
	{
		FWidgetPath LWidgetPath = FWidgetPath();
		if (FSlateApplication::Get().FindPathToWidget(CaptorWidget.ToSharedRef(), LWidgetPath, EVisibility::Visible))
		{
			FSlateApplication::Get().GetCursorUser()->SetCursorCaptor(CaptorWidget.ToSharedRef(), LWidgetPath);
			CaptorWidget.Reset();
		}
	}
}

SRMAImGuiContext::~SRMAImGuiContext()
{
	delete InputRouter;
	if (ImGui.Context) { ImGui::DestroyContext(ImGui.Context); }
	if (ImPlot.Context) { ImPlot::DestroyContext(ImPlot.Context); }
	ReleaseAllResources();
}

void SRMAImGuiContext::PrivateRegisterAttributes(FSlateAttributeInitializer& InInitializer) {}

void SRMAImGuiContext::Construct(const FArguments& InArgs)
{
	Flags = InArgs._Flags;
	UCName = InArgs._UCName.IsNone() ? "Default" : InArgs._UCName;
	SBackgroundBlur::Construct(SBackgroundBlur::FArguments());
	SetCanTick(true);
	if (RMAImGui::LoadSettings())
	{
		UE_LOG(LOG_RMAIMGUI, Display, TEXT("Context Initialize"));
		ImGui.Context = ImGui::CreateContext(&RMAImGui::Font::GAtlas);
		ImGui.IO = &ImGui::GetIO();
		ImPlot.Context = ImPlot::CreateContext();
		if (!RMAImGui::Font::IsBuilt()) { RMAImGui::Font::Build(); }
		if (ImGui.Context && RMAImGui::Font::IsBuilt() && RMAImGui::Font::AddResource(this))
		{
			ImGui.IO->IniFilename = nullptr;
			ImGui.IO->IniSavingRate = 2.f;
			ImGui.IO->ConfigWindowsMoveFromTitleBarOnly = true;
			ImGui.IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			ImGui.IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
			ImGui.IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			ImGui.IO->BackendFlags |= ImGuiBackendFlags_HasGamepad;
			ImGui.IO->BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
			LoadUCFile();
			InputRouter = new FInputRouter(this);
			UE_LOG(LOG_RMAIMGUI, Display, TEXT("Context Initialization Success"));
		}
		else
		{
			UE_LOG(LOG_RMAIMGUI, Warning, TEXT("Context Initialization Failed"));
			FRMAImGui::Get().ReleaseContext(SharedThis(this).ToWeakPtr());
		}
	}
}

int32 SRMAImGuiContext::OnPaint(const FPaintArgs& InArgs, const FGeometry& InGeometry, const FSlateRect& InRect, FSlateWindowElementList& OutDrawElements, int32 InLayerId, const FWidgetStyle& InWidgetStyle, bool InParentEnabled) const
{
	if (CanRender())
	{
#if STATS
		SCOPE_CYCLE_COUNTER(STAT_RENDER);
#endif
		ImGui::SetCurrentContext(ImGui.Context);
		ImPlot::SetCurrentContext(ImPlot.Context);
		TArray<FName> LResourcesInUse = {};
		const ImDrawData* LSrc = ImGui::GetDrawData();
		if (LSrc && LSrc->Valid)
		{
#if STATS
			SET_DWORD_STAT(STAT_BATCHES, LSrc->CmdListsCount);
			SET_DWORD_STAT(STAT_VERTICES, LSrc->TotalVtxCount);
			SET_DWORD_STAT(STAT_INDICES, LSrc->TotalIdxCount);
			SET_DWORD_STAT(STAT_TRIANGLES, (LSrc->TotalIdxCount > 0) ? LSrc->TotalIdxCount / 3 : 0);
#endif
			for (const ImDrawList* LBatch : LSrc->CmdLists)
			{
				TArray<FSlateVertex> LVertices = {};
				if (LBatch->VtxBuffer.Size > 0) { LVertices.AddDefaulted(LBatch->VtxBuffer.Size); }
				for (int32 LIndex = 0; LIndex < LVertices.Num(); LIndex++)
				{
					FSlateVertex& LVertex = LVertices[LIndex];
					LVertex.TexCoords[0] = LBatch->VtxBuffer[LIndex].uv.x;
					LVertex.TexCoords[1] = LBatch->VtxBuffer[LIndex].uv.y;
					LVertex.TexCoords[2] = 1.f;
					LVertex.TexCoords[3] = 1.f;
					LVertex.Position[0] = LBatch->VtxBuffer[LIndex].pos.x + InRect.Left;
					LVertex.Position[1] = LBatch->VtxBuffer[LIndex].pos.y + InRect.Top;
					LVertex.Color = FColor(LBatch->VtxBuffer[LIndex].col);
				}
				for (const ImDrawCmd& LCommand : LBatch->CmdBuffer)
				{
					if (LCommand.UserCallback)
					{
						LCommand.UserCallback(LBatch, &LCommand);
						continue;
					}
					if (LCommand.GetTexID().IsNone()) { continue; }
					TArray<SlateIndex> LIndexes = TArray<SlateIndex>(LBatch->IdxBuffer.Data + LCommand.IdxOffset, LCommand.ElemCount);
					OutDrawElements.PushClip(FSlateClippingZone(FSlateRect(
						LCommand.ClipRect.x, LCommand.ClipRect.y, LCommand.ClipRect.z, LCommand.ClipRect.w)
						.OffsetBy(FVector2f(InRect.Left, InRect.Top))));
					FSlateDrawElement::MakeCustomVerts(
						OutDrawElements,
						InLayerId + 1,
						GetResourceHandle(LCommand.GetTexID()),
						LVertices,
						LIndexes,
						nullptr,
						0,
						0);
					OutDrawElements.PopClip();
					LResourcesInUse.AddUnique(LCommand.GetTexID());
					LIndexes.Empty();
				}
			}
		}
		for (const FName LName : LResourcesInUse) { const_cast<SRMAImGuiContext*>(this)->StatResourceMemory(LName, true); }
	}
	return Super::OnPaint(InArgs, InGeometry, InRect, OutDrawElements, InLayerId, InWidgetStyle, InParentEnabled);
}

void SRMAImGuiContext::Tick(const FGeometry& InGeometry, const double InCurrentTime, const float InDeltaTime)
{
	for (auto LPair : Resources) { StatResourceMemory(LPair.Key); }
	BeginFrame(InGeometry, InDeltaTime);
	Super::Tick(InGeometry, InCurrentTime, InDeltaTime);
	EndFrame();
}

void SRMAImGuiContext::AddReferencedObjects(FReferenceCollector& InCollector)
{
	for (auto LPair : Resources)
	{
		TObjectPtr<UTexture2D> LTexture = Cast<UTexture2D>(LPair.Value->GetResourceObject());
		if (LTexture) { InCollector.AddReferencedObject(LTexture); }
	}
}

RMAImGui::EType SRMAImGuiContext::GetType() const
{
	if (GEngine && GEngine->GameViewport)
	{
		if (!GEngine->GameViewport->IsSimulateInEditorViewport())
		{
			if (TSharedPtr<SWidget> LWidget = FSlateApplication::Get().GetKeyboardFocusedWidget())
			{
				SWidget* LParent = GetParentWidget().Get();
				while (LParent)
				{
					if (LWidget.Get() == LParent)
					{
						return RMAImGui::EType::Gameplay;
					}
					LParent = LParent->GetParentWidget().Get();
				}
			}
		}
	}
	return RMAImGui::EType::Editor;
}

TSharedPtr<FSlateDynamicImageBrush> SRMAImGuiContext::GetResource(const FName InName) const
{
	if (Resources.Contains(InName)) { return *Resources.Find(InName); }
	return nullptr;
}

const FSlateResourceHandle& SRMAImGuiContext::GetResourceHandle(const FName InName) const
{
	if (Resources.Contains(InName))
	{
		const FSlateDynamicImageBrush* LResource = Resources.Find(InName)->Get();
		if (LResource) { return LResource->GetRenderingResource(); }
	}
	{
		static const FSlateResourceHandle GDefault = FSlateResourceHandle();
		return GDefault;
	}
}

bool SRMAImGuiContext::AddResource(const FName InName, UTexture2D* InTexture, const FVector2D InSize, const FLinearColor InTint)
{
	if (!InName.IsNone() && !Resources.Contains(InName) && InTexture)
	{
		const bool LRooted = InTexture->IsRooted();
		if (!LRooted) { InTexture->AddToRoot(); }
		Resources.Add(InName, MakeShareable(new FSlateDynamicImageBrush(
			InTexture,
			(InSize.Length() > 0) ? InSize : FVector2D(InTexture->GetSizeX(),
			InTexture->GetSizeY()),
			InName,
			InTint)));
		if (!LRooted) { InTexture->RemoveFromRoot(); }
		StatResource(InName);
		return true;
	}
	return false;
}

bool SRMAImGuiContext::AddResource(const FName InName, const FName InTexture, const FVector2D InSize, const FLinearColor InTint)
{
	if (!InName.IsNone() && !Resources.Contains(InName) && !InTexture.IsNone())
	{
		Resources.Add(InName, MakeShareable(new FSlateDynamicImageBrush(
			InTexture,
			InSize,
			InTint)));
		StatResource(InName);
		return true;
	}
	return false;
}

bool SRMAImGuiContext::AddResource(const FName InName, const TArray<uint8>& InTexture, const FVector2D InSize, const FLinearColor InTint)
{
	if (!InName.IsNone() && !Resources.Contains(InName) && !InTexture.IsEmpty())
	{
		Resources.Add(InName, (FSlateDynamicImageBrush::CreateWithImageData(InName, InSize, InTexture, InTint))->AsShared());
		StatResource(InName);
		return true;
	}
	return false;
}

bool SRMAImGuiContext::ReleaseResource(const FName InName)
{
	if (!InName.IsNone() && Resources.Contains(InName))
	{
		StatResource(InName, true);
		Resources.Remove(InName);
		return true;
	}
	return false;
}

void SRMAImGuiContext::ReleaseAllResources()
{
	TArray<FName> LKeys = {};
	Resources.GetKeys(LKeys);
	for (const FName LKey : LKeys)
	{
		ReleaseResource(LKey);
	}
}

void SRMAImGuiContext::StatResource(const FName InName, const bool InRelease)
{
#if STATS
	if (!InName.IsNone())
	{
		if (InRelease) { DEC_DWORD_STAT(STAT_RESOURCES); }
		else { INC_DWORD_STAT(STAT_RESOURCES); }
	}
#endif
}

void SRMAImGuiContext::StatResourceMemory(const FName InName, const bool InUse)
{
#if STATS
	if (!InName.IsNone())
	{
		const FSlateShaderResourceProxy* LProxy = GetResourceHandle(InName).GetResourceProxy();
		if (LProxy && LProxy->Resource)
		{
			const FSlateTexture2DRHIRef* LTexture = static_cast<FSlateTexture2DRHIRef*>(LProxy->Resource);
			if (LTexture)
			{
				const int64 LBytes = (LTexture->GetWidth() * LTexture->GetHeight()) * GPixelFormats[LTexture->GetPixelFormat()].BlockBytes;
				if (InUse) { INC_DWORD_STAT_BY(STAT_RESOURCES_IN_USE_MEMORY, LBytes); }
				else { INC_DWORD_STAT_BY(STAT_RESOURCES_MEMORY, LBytes); }
			}
		}
	}
#endif
}

bool SRMAImGuiContext::SaveUCFile()
{
	if (!UCName.IsNone())
	{
		if (ImGui.Context && ImGui.IO)
		{
			size_t LSize = 0;
			FString LString = "";
			const char* LSrc = ImGui::SaveIniSettingsToMemory(&LSize);
			FFileHelper::BufferToString(LString, (uint8*)LSrc, LSize);
			if (!LString.IsEmpty())
			{
				UE_LOG(LOG_RMAIMGUI, Display, TEXT("Saving User Cache"));
				return FFileHelper::SaveStringToFile(LString, *(FPaths::Combine(FPaths::ProjectSavedDir(), "Config", "RMAImGui") + "/UC" + UCName.ToString() + ".ini"));
			}
		}
	}
	return false;
}

bool SRMAImGuiContext::LoadUCFile()
{
	if (!UCName.IsNone())
	{
		if (ImGui.Context && ImGui.IO)
		{
			TArray<uint8> LSrc;
			FFileHelper::LoadFileToArray(LSrc, *(FPaths::Combine(FPaths::ProjectSavedDir(), "Config", "RMAImGui") + "/UC" + UCName.ToString() + ".ini"), FILEREAD_Silent);
			if (!LSrc.IsEmpty())
			{
				ImGui::LoadIniSettingsFromMemory((char*)LSrc.GetData(), LSrc.GetAllocatedSize());
				return true;
			}
			FFileHelper::LoadFileToArray(LSrc, *(FPaths::Combine(IPluginManager::Get().FindPlugin("RMAImGui")->GetBaseDir(), "Config") + "/UC" + UCName.ToString() + ".ini"));
			if (!LSrc.IsEmpty())
			{
				ImGui::LoadIniSettingsFromMemory((char*)LSrc.GetData(), LSrc.GetAllocatedSize());
				return true;
			}
		}
	}
	UE_LOG(LOG_RMAIMGUI, Warning, TEXT("User Cache Not Found"));
	return false;
}

bool SRMAImGuiContext::CanRender() const
{
	return FRMAImGui::Get().GetGlobalVisibility();
}

void SRMAImGuiContext::BeginFrame(const FGeometry& InGeometry, const float InDeltaTime)
{
	if (ImGui.Context && ImGui.IO)
	{
		ImGui::SetCurrentContext(ImGui.Context);
		ImPlot::SetCurrentContext(ImPlot.Context);
		ImGui.IO->DisplaySize.x = InGeometry.GetAbsoluteSize().X + 1.f;
		ImGui.IO->DisplaySize.y = InGeometry.GetAbsoluteSize().Y + 1.f;
		ImGui.IO->DeltaTime = InDeltaTime;
		if (ImGui.IO->WantSaveIniSettings) { ImGui.IO->WantSaveIniSettings = !SaveUCFile(); }
		ImGui::NewFrame();
		if (CanRender())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.f);
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.f);
			ImGui::SetNextWindowDockID(ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode), ImGuiCond_FirstUseEver);
			if (CanRouteInputs())
			{
				ImGui.IO->MouseDrawCursor = CanDrawCursor();
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, RMAImGui::GSettings->AlphaWhenFocused);
				SetBlurStrength(RMAImGui::GSettings->BlurStrength);
				SetVisibility(EVisibility::Visible);
			}
			else
			{
				ImGui.IO->MouseDrawCursor = false;
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, RMAImGui::GSettings->AlphaWhenNotFocused);
				SetBlurStrength(0.f);
				SetVisibility(EVisibility::HitTestInvisible);
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_HOOKS);
				for (auto LHook : Hooks) { LHook->Build(SharedThis(this).ToWeakPtr()); }
			}
		}
		else
		{
			SetBlurStrength(0.f);
			SetVisibility(EVisibility::HitTestInvisible);
		}
	}
}

void SRMAImGuiContext::EndFrame()
{
	if (ImGui.Context && ImGui.IO) { ImGui::Render(); }
}

bool SRMAImGuiContext::CanRouteInputs(const FKey InKey) const
{
	return CanRender() && FRMAImGui::Get().GetGlobalControls() && (HasFocus() || InKey == EKeys::Mouse2D);
}

bool SRMAImGuiContext::CanDrawCursor() const
{
	return RMAImGui::GSettings->DrawCursor && HasFocus();
}

bool SRMAImGuiContext::HasFocus() const
{
	if (GetType() == RMAImGui::EType::Gameplay) { return true; }
	return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered() || ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);
}
