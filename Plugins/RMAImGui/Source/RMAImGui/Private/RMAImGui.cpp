// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#include "RMAImGui.h"
#include "RMAImGuiHook.h"
#include "RMAImGuiSlate.h"
#include "RMAImGuiSettings.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

#if WITH_EDITOR
#include "SLevelViewport.h"
#include "LevelEditorViewport.h"
#endif

//Logs
DEFINE_LOG_CATEGORY(LOG_RMAIMGUI);

namespace RMAImGui
{
	namespace Slots
	{
		TMap<SWidget*, TMap<FName, TWeakPtr<SOverlay>>> GContainers = {};
		TWeakPtr<SOverlay> GetSlot(const FName& InName)
		{
			TWeakPtr<SOverlay> LResult = nullptr;
			GContainers.Remove(nullptr);
			if (!InName.IsNone())
			{
				TSharedPtr<SWidget> LWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
				if (!LWidget)
				{
					LWidget = FSlateApplication::Get().GetGameViewport();
				}
				if (LWidget)
				{
					if (auto LContainer = GContainers.Find(LWidget.Get()))
					{
						if (auto LSlot = LContainer->Find(InName))
						{
							LResult = *LSlot;
						}
					}
					if (!LResult.IsValid())
					{
						if (LWidget->GetType() == "SViewport")
						{
							if (LWidget->GetChildren()->GetChildAt(0)->GetType() == "SOverlay")
							{
								StaticCastSharedPtr<SOverlay>(LWidget->GetChildren()->
									GetChildAt(0).ToSharedPtr())->//SOverlay
									AddSlot().AttachWidget(SAssignNew(LResult, SOverlay));
							}
							else
							{
								StaticCastSharedPtr<SOverlay>(LWidget->GetChildren()->
									GetChildAt(0)->GetChildren()->//SGameLayerManager
									GetChildAt(0)->GetChildren()->//SDPIScaler
									GetChildAt(0)->GetChildren()->//SVerticalBox
									GetChildAt(1).ToSharedPtr())->//SOverlay
									AddSlot().AttachWidget(SAssignNew(LResult, SOverlay));
							}
						}
#if WITH_EDITOR
						else if (LWidget->GetType() == "SPIEViewport")
						{
							StaticCastSharedPtr<SOverlay>(LWidget->GetChildren()->
								GetChildAt(0)->GetChildren()->//SGameLayerManager
								GetChildAt(0)->GetChildren()->//SDPIScaler
								GetChildAt(0)->GetChildren()->//SVerticalBox
								GetChildAt(1).ToSharedPtr())->//SOverlay
								AddSlot().AttachWidget(SAssignNew(LResult, SOverlay));
						}
#endif
						if (LResult.IsValid())
						{
							GContainers.FindOrAdd(LWidget.Get()).FindOrAdd(InName) = LResult;
						}
					}
				}
			}
			return LResult;
		}
	}
	namespace Commands
	{
#if WITH_RMAIMGUI_DEMOS
		static FAutoConsoleCommand ToggleDemos(TEXT("RMAImGui.ToggleDemos"), TEXT(""),
			FConsoleCommandDelegate::CreateLambda(
				[]()
				{
					auto LSlot = Slots::GetSlot("Demos");
					if (LSlot.IsValid())
					{
						if (FRMAImGui::Get().FindContext("Demos").IsValid()) { FRMAImGui::Get().ReleaseContext("Demos"); }
						else
						{
							TWeakPtr<SRMAImGuiContext> LContext = FRMAImGui::Get().CreateContext("Demos", LSlot.Pin().ToSharedRef());
							if (LContext.IsValid())
							{
								LContext.Pin()->AddHook(new FRMAImGuiHook([](TWeakPtr<SRMAImGuiContext> InContext) { ImGui::ShowDemoWindow(); }));
								LContext.Pin()->AddHook(new FRMAImGuiHook([](TWeakPtr<SRMAImGuiContext> InContext) { ImPlot::ShowDemoWindow(); }));
								LContext.Pin()->AddHook(new FRMAImGuiHook([](TWeakPtr<SRMAImGuiContext> InContext) { ImSpinner::demoSpinners(); }));
							}
						}
					}
				}));
#endif
		static FAutoConsoleCommand ToggleVisibility(TEXT("RMAImGui.ToggleVisibility"), TEXT(""),
			FConsoleCommandDelegate::CreateLambda(
				[]() { FRMAImGui::Get().GlobalVisibility = !FRMAImGui::Get().GlobalVisibility; }));
		static FAutoConsoleCommand ToggleControls(TEXT("RMAImGui.ToggleControls"), TEXT(""),
			FConsoleCommandDelegate::CreateLambda(
				[]() { FRMAImGui::Get().GlobalControls = !FRMAImGui::Get().GlobalControls; }));
	}
}

bool FRMAImGui::FInputMaster::RouteKey(const FKey InKey, const EInputEvent InEvent, const bool InUppercase)
{
	SCOPE_CYCLE_COUNTER(STAT_INPUTS);
	if (InEvent == EInputEvent::IE_Pressed || InEvent == EInputEvent::IE_Repeat || InEvent == EInputEvent::IE_DoubleClick)
	{
		Pressed.Add(InKey);
		if (InEvent == EInputEvent::IE_Pressed || InEvent == EInputEvent::IE_DoubleClick)
		{
#if WITH_RMAIMGUI_DEMOS
			//Toggle Demos
			{
				for (const auto LPair : GetMutableDefault<URMAImGuiSettings>()->ToggleDemos)
				{
					if (LPair.Contains(InKey) && LPair.Match(Pressed)) { GEngine->Exec(nullptr, TEXT("RMAImGui.ToggleDemos")); return true; }
				}
			}
#endif
			//Toggle Visibility
			{
				for (const auto LPair : GetMutableDefault<URMAImGuiSettings>()->ToggleVisibility)
				{
					if (LPair.Contains(InKey) && LPair.Match(Pressed)) { GEngine->Exec(nullptr, TEXT("RMAImGui.ToggleVisibility")); return true; }
				}
			}
			//Toggle Controls
			{
				for (const auto LPair : GetMutableDefault<URMAImGuiSettings>()->ToggleControls)
				{
					if (LPair.Contains(InKey) && LPair.Match(Pressed)) { GEngine->Exec(nullptr, TEXT("RMAImGui.ToggleControls")); return true; }
				}
			}
		}
	}
	else { Pressed.Remove(InKey); }
	if (CanRoute())
	{
		for (auto LPair : FRMAImGui::Get().Contexts)
		{
			if (LPair.Value.IsValid() && LPair.Value.Pin()->GetInputRouter())
			{
				if (LPair.Value.Pin()->GetInputRouter()->RouteKey(InKey, InEvent, InUppercase)) { return true; }
			}
		}
	}
	return false;
}

bool FRMAImGui::FInputMaster::RouteAxis(const FKey InKey, const float InDelta)
{
	SCOPE_CYCLE_COUNTER(STAT_INPUTS);
	if (CanRoute())
	{
		for (auto LPair : FRMAImGui::Get().Contexts)
		{
			if (LPair.Value.IsValid() && LPair.Value.Pin()->GetInputRouter())
			{
				if (LPair.Value.Pin()->GetInputRouter()->RouteAxis(InKey, InDelta)) { return true; }
			}
		}
	}
	return false;
}

bool FRMAImGui::FInputMaster::RouteCursor(const FVector2D InDelta, TSharedPtr<ICursor> InCursor)
{
	SCOPE_CYCLE_COUNTER(STAT_INPUTS);
	if (CanRoute())
	{
		for (auto LPair : FRMAImGui::Get().Contexts)
		{
			if (LPair.Value.IsValid() && LPair.Value.Pin()->GetInputRouter())
			{
				if (LPair.Value.Pin()->GetInputRouter()->RouteCursor(InDelta, InCursor)) { return true; }
			}
		}
	}
	return false;
}

void FRMAImGui::StartupModule()
{
	LoadSettings();
	if (!InputMaster.IsValid() && FSlateApplication::IsInitialized())
	{
		InputMaster = MakeShareable(new FInputMaster());
		FSlateApplication::Get().RegisterInputPreProcessor(InputMaster);
	}
}

void FRMAImGui::ShutdownModule()
{
	ShuttingDown = true;
	ReleaseAllContexts();
	InputMaster.Reset();
}

void FRMAImGui::LoadSettings()
{
	const auto LSettings = GetMutableDefault<URMAImGuiSettings>();
	if (LSettings)
	{
		GlobalVisibility = LSettings->DefaultVisibility;
		GlobalControls = LSettings->DefaultControls;
		UE_LOG(LOG_RMAIMGUI, Display, TEXT("Loaded Settings"));
	}
}

TWeakPtr<SRMAImGuiContext> FRMAImGui::FindContext(const FName InName)
{
	if (!InName.IsNone()) { for (auto LPair : Contexts) { if (LPair.Key == InName) { return LPair.Value; } } }
	return nullptr;
}

TWeakPtr<SRMAImGuiContext> FRMAImGui::CreateContext(const FName InName, TSharedRef<SOverlay> InSlot, const int32 InZOrder, const int32 InFlags, const FName InUCName)
{
	if (!InName.IsNone())
	{
		if (FindContext(InName).IsValid()) { return FindContext(InName); }
		InSlot.Get().AddSlot(InZOrder).AttachWidget(
			SAssignNew(Contexts.Add(InName, nullptr), SRMAImGuiContext)
			.UCName(InUCName)
			.Flags(InFlags));
		UE_LOG(LOG_RMAIMGUI, Display, TEXT("Creating Context [%s]"), *InName.ToString());
		if (Contexts.Contains(InName)) { return *Contexts.Find(InName); }
	}
	return nullptr;
}

TWeakPtr<SRMAImGuiContext> FRMAImGui::CreateContext(const FName InName, const int32 InZOrder, const int32 InFlags, const FName InUCName)
{
	auto LSlot = RMAImGui::Slots::GetSlot();
	return CreateContext(InName, LSlot.Pin().ToSharedRef(), InZOrder, InFlags, InUCName);
}

void FRMAImGui::ReleaseContext(const FName InName)
{
	if (Contexts.Contains(InName))
	{
		if (!IsShuttingDown())
		{
			TWeakPtr<SRMAImGuiContext> LContext = *Contexts.Find(InName);
			if (LContext.IsValid())
			{
				TSharedPtr<SOverlay> LParent = StaticCastSharedPtr<SOverlay>(LContext.Pin()->GetParentWidget());
				if (LParent.IsValid())
				{
					LParent->RemoveSlot(LContext.Pin().ToSharedRef());
					if (LParent->GetChildren()->Num() <= 0)
					{
						StaticCastSharedPtr<SOverlay>(LParent->GetParentWidget())->RemoveSlot(LParent.ToSharedRef());
					}
				}
			}
		}
		Contexts.Remove(InName);
		UE_LOG(LOG_RMAIMGUI, Display, TEXT("Releasing Context [%s]"), *InName.ToString());
	}
}

void FRMAImGui::ReleaseContext(TWeakPtr<SRMAImGuiContext> InContext)
{
	if (InContext.IsValid())
	{
		for (auto LPair : Contexts) { if (LPair.Value == InContext) { return ReleaseContext(LPair.Key); } }
	}
}

void FRMAImGui::ReleaseAllContexts()
{
	while (!Contexts.IsEmpty())
	{
		const auto LMap = Contexts;
		for (auto LPair : LMap)
		{
			ReleaseContext(LPair.Key);
		}
	}
}

IMPLEMENT_MODULE(FRMAImGui, RMAImGui)
