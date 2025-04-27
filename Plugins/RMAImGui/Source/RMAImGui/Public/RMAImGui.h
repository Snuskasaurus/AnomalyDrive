// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Engine/EngineBaseTypes.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/IInputProcessor.h"
#include "RMAImGuiDef.h"

class SOverlay;
class SRMAImGuiContext;

namespace RMAImGui
{
	enum class EType : uint8
	{
		None,
		Gameplay,
		Editor
	};
	enum EFlags
	{
		None = 0
	};
	namespace Slots
	{
		TWeakPtr<SOverlay> GetSlot(const FName& InName = "Default");
	}
}

//Module Runtime
class FRMAImGui : public IModuleInterface
{

public:

	//Return The Module Instance
	static inline FRMAImGui& Get() { return FModuleManager::LoadModuleChecked<FRMAImGui>("RMAImGui"); }

	//Return The Module Was Loaded
	static inline bool IsAvailable() { return FModuleManager::Get().IsModuleLoaded("RMAImGui"); }

	//Called After The Module Object Has Been Loaded
	virtual void StartupModule() override;

	//Called Before The Module Is Unloaded
	virtual void ShutdownModule() override;

	//Is Shutting Down
	bool IsShuttingDown() const { return ShuttingDown; };

private:

	class FInputMaster : public IInputProcessor
	{
	
	public:

		virtual void Tick(const float InDeltaTime, FSlateApplication& InSlateApp, TSharedRef<ICursor> InCursor) override
		{
			RouteCursor(FVector2D(0.f), InCursor);
		};

		virtual bool HandleKeyDownEvent(FSlateApplication& InSlateApp, const FKeyEvent& InKeyEvent) override
		{
			return RouteKey(InKeyEvent.GetKey(), InKeyEvent.IsRepeat() ? EInputEvent::IE_Repeat : EInputEvent::IE_Pressed, !((!InKeyEvent.AreCapsLocked()) == (!InKeyEvent.IsShiftDown())));
		};

		virtual bool HandleKeyUpEvent(FSlateApplication& InSlateApp, const FKeyEvent& InKeyEvent) override
		{
			return RouteKey(InKeyEvent.GetKey(), EInputEvent::IE_Released);
		};

		virtual bool HandleMouseMoveEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent) override
		{
			return RouteCursor(InMouseEvent.GetCursorDelta());
		};

		virtual bool HandleMouseButtonDownEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent) override
		{
			return RouteKey(InMouseEvent.GetEffectingButton(), InMouseEvent.IsRepeat() ? EInputEvent::IE_Repeat : EInputEvent::IE_Pressed);
		};

		virtual bool HandleMouseButtonUpEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent) override
		{
			return RouteKey(InMouseEvent.GetEffectingButton(), EInputEvent::IE_Released);
		};

		virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& InSlateApp, const FPointerEvent& InMouseEvent) override
		{
			return RouteKey(InMouseEvent.GetEffectingButton(), EInputEvent::IE_DoubleClick);
		};

		virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& InSlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
		{
			return RouteAxis(EKeys::MouseWheelAxis, InWheelEvent.GetWheelDelta());
		};

		bool CanRoute() const { return true; };

		bool RouteKey(const FKey InKey, const EInputEvent InEvent, const bool InUppercase = false);

		bool RouteAxis(const FKey InKey, const float InDelta);

		bool RouteCursor(const FVector2D InDelta, TSharedPtr<ICursor> InCursor = nullptr);

	private:

		TArray<FKey> Pressed = {};

	};

	//Load Settings
	void LoadSettings();

	//Contexts
	TMap<FName, TWeakPtr<SRMAImGuiContext>> Contexts;

	//Input Master
	TSharedPtr<FInputMaster> InputMaster;

	//Shutting Down
	bool ShuttingDown = false;

	friend class FInputMaster;

public:

	//Global
	bool GlobalVisibility = false;
	bool GlobalControls = false;

public:

	//Find Context
	RMAIMGUI_API TWeakPtr<SRMAImGuiContext> FindContext(const FName InName);

	//Create Context
	RMAIMGUI_API TWeakPtr<SRMAImGuiContext> CreateContext(const FName InName, TSharedRef<SOverlay> InSlot, const int32 InZOrder = INDEX_NONE, const int32 InFlags = 0, const FName InUCName = "");
	
	//Create Context
	RMAIMGUI_API TWeakPtr<SRMAImGuiContext> CreateContext(const FName InName, const int32 InZOrder = INDEX_NONE, const int32 InFlags = 0, const FName InUCName = "");

	//Release Context
	RMAIMGUI_API void ReleaseContext(const FName InName);

	//Release Context
	RMAIMGUI_API void ReleaseContext(TWeakPtr<SRMAImGuiContext> InContext);

	//Release All Contexts
	RMAIMGUI_API void ReleaseAllContexts();

	//Getter Global Visibility
	RMAIMGUI_API bool GetGlobalVisibility() const { return GlobalVisibility; };

	//Getter Global Controls
	RMAIMGUI_API bool GetGlobalControls() const { return GlobalControls; };

};
