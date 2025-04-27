// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#pragma once
#include "RMAImGui.h"
#include "Widgets/Layout/SBackgroundBlur.h"

class FRMAImGuiHook;
class UTexture2D;
struct FSlateDynamicImageBrush;

//Context
class RMAIMGUI_API SRMAImGuiContext : public SBackgroundBlur, public FGCObject
{

	SLATE_DECLARE_WIDGET(SRMAImGuiContext, SBackgroundBlur)

public:

	SLATE_BEGIN_ARGS(SRMAImGuiContext)
	{
		_Visibility = EVisibility::HitTestInvisible;
	}
	SLATE_ARGUMENT(FName, UCName)
	SLATE_ARGUMENT(int32, Flags)
	SLATE_END_ARGS()

	virtual ~SRMAImGuiContext();

public:
	
	class FInputRouter
	{

	public:

		FInputRouter() = delete;
		FInputRouter(SRMAImGuiContext* InContext) { Context = InContext; };
		~FInputRouter() { RestoreCaptor(); };

		//Route Key
		bool RouteKey(const FKey InKey, const EInputEvent InEvent, const bool InUppercase = false);

		//Route Axis
		bool RouteAxis(const FKey InKey, const float InDelta);

		//Route Cursor
		bool RouteCursor(const FVector2D InDelta, TSharedPtr<ICursor> InCursor = nullptr);

		//Stack In
		void StackIn(const FKey InKey, const EInputEvent InEvent, const bool InAPI);

		//Stack Out
		bool StackOut(const FKey InKey, const EInputEvent InEvent, const bool InAPI);

		//Replace Key
		FKey ReplaceKey(const FKey InKey) const;

		//Getter API Key
		int GetAPIKey(const FKey InKey) const;

		//Getter API Axis
		int GetAPIAxis(const FKey InKey) const;

		//Getter API Char
		int GetAPIChar(const FKey InKey) const;

		//Getter API Modifier
		int GetAPIModifier(const FKey InKey) const;

		//Can Route
		bool CanRoute(const FKey InKey = EKeys::AnyKey) const;

		//Is Modifier Pressed
		bool IsModifierPressed() const;

	private:

		//Restore Captor
		void RestoreCaptor();

	protected:

		//Context
		SRMAImGuiContext* Context = nullptr;

		//Stack API
		TMap<FKey, EInputEvent> StackAPI;

		//Stack Engine
		TMap<FKey, EInputEvent> StackEngine;

		//Captor Widget
		TSharedPtr<SWidget> CaptorWidget = nullptr;

	};

	struct FImGui
	{

	public:

		//IO
		ImGuiIO* IO = nullptr;

		//Context
		ImGuiContext* Context = nullptr;

	};

	struct FImPlot
	{

	public:

		//Context
		ImPlotContext* Context = nullptr;

	};

public:

	//Construct
	void Construct(const FArguments& InArgs);

	//On Paint
	virtual int32 OnPaint(const FPaintArgs& InArgs, const FGeometry& InGeometry, const FSlateRect& InRect, FSlateWindowElementList& OutDrawElements, int32 InLayerId, const FWidgetStyle& InWidgetStyle, bool InParentEnabled) const override;

	//Tick
	virtual void Tick(const FGeometry& InGeometry, const double InCurrentTime, const float InDeltaTime) override;

	//Add Referenced Objects
	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override;

	//Getter Referencer Name
	virtual FString GetReferencerName() const override { return TEXT("SRMAImGuiContext"); };

private:

	//Flags
	int32 Flags = RMAImGui::EFlags::None;

	//UC Name
	FName UCName = "";

	//Hooks
	TArray<FRMAImGuiHook*> Hooks = {};

	//ImGui
	FImGui ImGui = FImGui();

	//ImPlot
	FImPlot ImPlot = FImPlot();

	//Input Router
	FInputRouter* InputRouter = nullptr;

	//Resources
	TMap<FName, TSharedPtr<FSlateDynamicImageBrush>> Resources = {};

public:

	//Getter Type
	RMAImGui::EType GetType() const;

	//Contains Flag
	bool ContainsFlag(const int InFlag) const { return (Flags & InFlag); };

	//Getter ImGui IO
	ImGuiIO* GetIOImGui() { return ImGui.IO; };

	//Getter ImGui Context
	ImGuiContext* GetContextImGui() { return ImGui.Context; };

	//Getter ImPlot Context
	ImPlotContext* GetContextImPlot() { return ImPlot.Context; };

	//Getter Input Router
	FInputRouter* GetInputRouter() const { return InputRouter; };

	//Add Hook
	void AddHook(FRMAImGuiHook* InHook) { Hooks.Add(InHook); };

	//Remove Hook
	void RemoveHook(FRMAImGuiHook* InHook) { Hooks.Remove(InHook); };

	//Flush Hooks
	void FlushHooks() { Hooks.Empty(); };

	//Getter Resource
	TSharedPtr<FSlateDynamicImageBrush> GetResource(const FName InName) const;

	//Getter Resource Handle
	const FSlateResourceHandle& GetResourceHandle(const FName InName) const;

	//Add Resource
	bool AddResource(const FName InName, UTexture2D* InTexture, const FVector2D InSize = FVector2D(0.f), const FLinearColor InTint = FLinearColor::White);

	//Add Resource
	bool AddResource(const FName InName, const FName InTexture, const FVector2D InSize, const FLinearColor InTint = FLinearColor::White);

	//Add Resource
	bool AddResource(const FName InName, const TArray<uint8>& InTexture, const FVector2D InSize, const FLinearColor InTint = FLinearColor::White);

	//Release Resource
	bool ReleaseResource(const FName InName);

	//Release All Resources
	void ReleaseAllResources();
	
private:
	
	//Stat Resource
	void StatResource(const FName InName, const bool InRelease = false);

	//Stat Resource Memory
	void StatResourceMemory(const FName InName, const bool InUse = false);

protected:

	//Save UC File
	virtual bool SaveUCFile();

	//Load UC File
	virtual bool LoadUCFile();

	//Can Render
	virtual bool CanRender() const;

	//Begin Frame
	virtual void BeginFrame(const FGeometry& InGeometry, const float InDeltaTime);

	//End Frame
	virtual void EndFrame();

	//Can Route Inputs
	virtual bool CanRouteInputs(const FKey InKey = EKeys::AnyKey) const;

	//Can Draw Cursor
	virtual bool CanDrawCursor() const;

	//Has Focus
	virtual bool HasFocus() const;

	//Getter Cursor Type
	virtual TOptional<EMouseCursor::Type> GetCursor() const override { return EMouseCursor::Type::None; };

};
