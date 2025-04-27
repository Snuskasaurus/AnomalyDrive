// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#pragma once
#include <functional>
#include "RMAImGuiSlate.h"
#include "RMAImGuiUtility.h"
#include "BlueprintNodeHelpers.h"
#include "RMAImGuiHook.generated.h"

namespace RMAImGui
{
#if RMAIMGUI_LIBRARY
	namespace Library
	{
		namespace Enum
		{
			class RMAIMGUI_API _ImGuiWindowFlags_
			{
			public: static ImGuiWindowFlags_ Make(const TArray<FName> InFlags)
				{
					TMap<FName, ImGuiWindowFlags_> LMap;
					ImGuiWindowFlags_ LResult = ImGuiWindowFlags_::ImGuiWindowFlags_None;
					{
						LMap.Add("None", ImGuiWindowFlags_None);
						LMap.Add("NoTitleBar", ImGuiWindowFlags_NoTitleBar);
						LMap.Add("NoResize", ImGuiWindowFlags_NoResize);
						LMap.Add("NoMove", ImGuiWindowFlags_NoMove);
						LMap.Add("NoScrollbar", ImGuiWindowFlags_NoScrollbar);
						LMap.Add("NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse);
						LMap.Add("NoCollapse", ImGuiWindowFlags_NoCollapse);
						LMap.Add("AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize);
						LMap.Add("NoBackground", ImGuiWindowFlags_NoBackground);
						LMap.Add("NoSavedSettings", ImGuiWindowFlags_NoSavedSettings);
						LMap.Add("NoMouseInputs", ImGuiWindowFlags_NoMouseInputs);
						LMap.Add("MenuBar", ImGuiWindowFlags_MenuBar);
						LMap.Add("HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar);
						LMap.Add("NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing);
						LMap.Add("NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus);
						LMap.Add("AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar);
						LMap.Add("AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar);
						LMap.Add("AlwaysUseWindowPadding", ImGuiWindowFlags_AlwaysUseWindowPadding);
						LMap.Add("NoNavInputs", ImGuiWindowFlags_NoNavInputs);
						LMap.Add("NoNavFocus", ImGuiWindowFlags_NoNavFocus);
						LMap.Add("UnsavedDocument", ImGuiWindowFlags_UnsavedDocument);
						LMap.Add("NoDocking", ImGuiWindowFlags_NoDocking);
						LMap.Add("NoNav", ImGuiWindowFlags_NoNav);
						LMap.Add("NoDecoration", ImGuiWindowFlags_NoDecoration);
						LMap.Add("NoInputs", ImGuiWindowFlags_NoInputs);
					}
					for (const FName LFlag : InFlags)
					{
						const auto LValue = LMap.Find(LFlag);
						if (LValue) { LResult = (ImGuiWindowFlags_)(LResult | *LValue); }
					}
					return LResult;
				}
			};

			class RMAIMGUI_API _ImGuiSliderFlags_
			{
			public: static ImGuiSliderFlags_ Make(const TArray<FName> InFlags)
			{
				TMap<FName, ImGuiSliderFlags_> LMap;
				ImGuiSliderFlags_ LResult = ImGuiSliderFlags_::ImGuiSliderFlags_None;
				{
					LMap.Add("None", ImGuiSliderFlags_None);
					LMap.Add("AlwaysClamp", ImGuiSliderFlags_AlwaysClamp);
					LMap.Add("Logarithmic", ImGuiSliderFlags_Logarithmic);
					LMap.Add("NoRoundToFormat", ImGuiSliderFlags_NoRoundToFormat);
					LMap.Add("NoInput", ImGuiSliderFlags_NoInput);
				}
				for (const FName LFlag : InFlags)
				{
					const auto LValue = LMap.Find(LFlag);
					if (LValue) { LResult = (ImGuiSliderFlags_)(LResult | *LValue); }
				}
				return LResult;
			}
			};

			class RMAIMGUI_API _ImGuiTreeNodeFlags_
			{
			public: static ImGuiTreeNodeFlags_ Make(const TArray<FName> InFlags)
			{
				TMap<FName, ImGuiTreeNodeFlags_> LMap;
				ImGuiTreeNodeFlags_ LResult = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_None;
				{
					LMap.Add("None", ImGuiTreeNodeFlags_None);
					LMap.Add("Selected", ImGuiTreeNodeFlags_Selected);
					LMap.Add("Framed", ImGuiTreeNodeFlags_Framed);
					LMap.Add("AllowOverlap", ImGuiTreeNodeFlags_AllowOverlap);
					LMap.Add("NoTreePushOnOpen", ImGuiTreeNodeFlags_NoTreePushOnOpen);
					LMap.Add("NoAutoOpenOnLog", ImGuiTreeNodeFlags_NoAutoOpenOnLog);
					LMap.Add("DefaultOpen", ImGuiTreeNodeFlags_DefaultOpen);
					LMap.Add("OpenOnDoubleClick", ImGuiTreeNodeFlags_OpenOnDoubleClick);
					LMap.Add("OpenOnArrow", ImGuiTreeNodeFlags_OpenOnArrow);
					LMap.Add("Leaf", ImGuiTreeNodeFlags_Leaf);
					LMap.Add("Bullet", ImGuiTreeNodeFlags_Bullet);
					LMap.Add("FramePadding", ImGuiTreeNodeFlags_FramePadding);
					LMap.Add("SpanAvailWidth", ImGuiTreeNodeFlags_SpanAvailWidth);
					LMap.Add("SpanFullWidth", ImGuiTreeNodeFlags_SpanFullWidth);
					LMap.Add("NavLeftJumpsBackHere", ImGuiTreeNodeFlags_NavLeftJumpsBackHere);
					LMap.Add("CollapsingHeader", ImGuiTreeNodeFlags_CollapsingHeader);
				}
				for (const FName LFlag : InFlags)
				{
					const auto LValue = LMap.Find(LFlag);
					if (LValue) { LResult = (ImGuiTreeNodeFlags_)(LResult | *LValue); }
				}
				return LResult;
			}
			};
		}
	}
#endif
}

//Hook
class FRMAImGuiHook
{

public:
	
	FRMAImGuiHook() {};
	FRMAImGuiHook(std::function<void(TWeakPtr<SRMAImGuiContext>)> InBuild) { Build = InBuild; };

	//Build Function
	std::function<void(TWeakPtr<SRMAImGuiContext>)> Build = [](TWeakPtr<SRMAImGuiContext> InContext) {};

};

//Hook (Blueprint) #WIP#
UCLASS(abstract, Blueprintable)
class RMAIMGUI_API URMAImGuiHook : public UObject, public FRMAImGuiHook
{

    GENERATED_BODY()

public:

    URMAImGuiHook()
	{
		HasBuildBP = BlueprintNodeHelpers::HasBlueprintFunction(TEXT("BuildBP"), *this, *StaticClass());
		Build = [&](TWeakPtr<SRMAImGuiContext> InContext) { if (HasBuildBP) { BuildBP(); } };
	}

    //Build BP
    UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "OnBuild"))
		void BuildBP();

private:

	//Has Build BP
	bool HasBuildBP = false;

public:

	//Show Demo Window
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Open"))
		void ShowDemoWindow(UPARAM(ref) bool& Open) { ImGui::ShowDemoWindow(&Open); };

	//Show Metrics Window
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Open"))
		void ShowMetricsWindow(UPARAM(ref) bool& Open) { ImGui::ShowMetricsWindow(&Open); };

	//Show Debug Log Window
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Open"))
		void ShowDebugLogWindow(UPARAM(ref) bool& Open) { ImGui::ShowDebugLogWindow(&Open); };

	//Show Stack Tool Window
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Open"))
		void ShowStackToolWindow(UPARAM(ref) bool& Open) { ImGui::ShowStackToolWindow(&Open); };

	//Show About Window
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Open"))
		void ShowAboutWindow(UPARAM(ref) bool& Open) { ImGui::ShowAboutWindow(&Open); };

	//Style Colors Dark
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void StyleColorsDark() { ImGui::StyleColorsDark(); };

	//Style Colors Light
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void StyleColorsLight() { ImGui::StyleColorsLight(); };

	//Style Colors Classic
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void StyleColorsClassic() { ImGui::StyleColorsClassic(); };

	//Begin
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Open"))
		bool Begin(const FString& Name, const TArray<FName> Flags, UPARAM(ref) bool& Open)
	{
#if RMAIMGUI_LIBRARY
		if (Open)
		{
			return ImGui::Begin(TCHAR_TO_ANSI(*Name), &Open, RMAImGui::Library::Enum::_ImGuiWindowFlags_::Make(Flags));
		}
#endif
		return false;
	};

	//End
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void End() { ImGui::End(); };

	//Begin Child
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags"))
		bool BeginChild(const FString& ID, const TArray<FName> Flags, const FVector2D Size = FVector2D(0, 0), const bool Border = false)
	{
#if RMAIMGUI_LIBRARY
		return ImGui::BeginChild(TCHAR_TO_ANSI(*ID), ImVec2(Size.X, Size.Y), Border, RMAImGui::Library::Enum::_ImGuiWindowFlags_::Make(Flags));
#endif
		return false;
	};

	//End Child
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void EndChild() { ImGui::EndChild(); };

	//Separator
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Separator() { ImGui::Separator(); };

	//SameLine
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void SameLine(const float Offset = 0.0f, const float Spacing = -1.0f) { ImGui::SameLine(Offset, Spacing); };

	//New Line
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void NewLine() { ImGui::NewLine(); };

	//Spacing
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Spacing() { ImGui::Spacing(); };

	//Dummy
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Dummy(const FVector2D Size) { ImGui::Dummy(ImVec2(Size.X, Size.Y)); };

	//Indent
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Indent(const float Spacing = 0.0f) { ImGui::Indent(Spacing); };

	//Unindent
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Unindent(const float Spacing = 0.0f) { ImGui::Unindent(Spacing); };

	//Begin Group
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void BeginGroup() { ImGui::BeginGroup(); };

	//End Group
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void EndGroup() { ImGui::EndGroup(); };

	//Text
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Text(const FString& Text) { ImGui::Text(TCHAR_TO_ANSI(*Text)); };

	//Text Colored
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void TextColored(const FString& Text, const FLinearColor Color = FLinearColor::White)
	{
		ImGui::TextColored(ImVec4(Color.R, Color.G, Color.B, Color.A), TCHAR_TO_ANSI(*Text));
	};

	//Text Disabled
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void TextDisabled(const FString& Text) { ImGui::TextDisabled(TCHAR_TO_ANSI(*Text)); };

	//Text Wrapped
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void TextWrapped(const FString& Text) { ImGui::TextWrapped(TCHAR_TO_ANSI(*Text)); };

	//Label Text
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void LabelText(const FString& Label, const FString& Text) { ImGui::LabelText(TCHAR_TO_ANSI(*Label), TCHAR_TO_ANSI(*Text)); };

	//Bullet Text
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void BulletText(const FString& Text) { ImGui::BulletText(TCHAR_TO_ANSI(*Text)); };

	//Separator Text
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void SeparatorText(const FString& Label) { ImGui::SeparatorText(TCHAR_TO_ANSI(*Label)); };

	//Button
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		bool Button(const FString& Label, const FVector2D Size) { return ImGui::Button(TCHAR_TO_ANSI(*Label), ImVec2(Size.X, Size.Y)); };

	//Small Button
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		bool SmallButton(const FString& Label) { return ImGui::SmallButton(TCHAR_TO_ANSI(*Label)); };

	//Checkbox
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Value"))
		bool Checkbox(const FString& Label, UPARAM(ref) bool& Value) { return ImGui::Checkbox(TCHAR_TO_ANSI(*Label), &Value); };

	//Radio Button
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Active"))
		bool RadioButton(const FString& Label, UPARAM(ref) bool& Active) { return ImGui::RadioButton(TCHAR_TO_ANSI(*Label), Active); };

	//Progress Bar
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void ProgressBar(const float Fraction, const FVector2D Size) { ImGui::ProgressBar(Fraction, ImVec2(Size.X, Size.Y)); };

	//Bullet
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void Bullet() { ImGui::Bullet(); };

	//Drag Float
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool DragFloat(const FString& Label, const TArray<FName> Flags, UPARAM(ref) float& Value, const float ValueSpeed = 1.0f, const float ValueMin = 0.0f, const float ValueMax = 0.0f, const FString Format = "%.3f")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::DragFloat(TCHAR_TO_ANSI(*Label), &Value, ValueSpeed, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//Drag Int
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool DragInt(const FString& Label, const TArray<FName> Flags, UPARAM(ref) int& Value, const float ValueSpeed = 1.0f, const int ValueMin = 0, const int ValueMax = 0, const FString Format = "%d")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::DragInt(TCHAR_TO_ANSI(*Label), &Value, ValueSpeed, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//Slider Float
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool SliderFloat(const FString& Label, const TArray<FName> Flags, UPARAM(ref) float& Value, const float ValueMin = 0.0f, const float ValueMax = 0.0f, const FString Format = "%.3f")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::SliderFloat(TCHAR_TO_ANSI(*Label), &Value, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//Slider Angle
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, ValueRad"))
		bool SliderAngle(const FString& Label, const TArray<FName> Flags, UPARAM(ref) float& ValueRad, const float DegreesMin = -360.0f, const float DegreesMax = +360.0f, const FString Format = "%.0f deg")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::SliderAngle(TCHAR_TO_ANSI(*Label), &ValueRad, DegreesMin, DegreesMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//Slider Int
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool SliderInt(const FString& Label, const TArray<FName> Flags, UPARAM(ref) int& Value, const int ValueMin = 0, const int ValueMax = 0, const FString Format = "%d")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::SliderInt(TCHAR_TO_ANSI(*Label), &Value, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//V Slider Float
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool VSliderFloat(const FString& Label, const TArray<FName> Flags, const FVector2D Size, UPARAM(ref) float& Value, const float ValueMin = 0.0f, const float ValueMax = 0.0f, const FString Format = "%.3f")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::VSliderFloat(TCHAR_TO_ANSI(*Label), ImVec2(Size.X, Size.Y), &Value, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//V Slider Int
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags, Value"))
		bool VSliderInt(const FString& Label, const TArray<FName> Flags, const FVector2D Size, UPARAM(ref) int& Value, const int ValueMin = 0, const int ValueMax = 0, const FString Format = "%d")
	{
#if RMAIMGUI_LIBRARY
		return ImGui::VSliderInt(TCHAR_TO_ANSI(*Label), ImVec2(Size.X, Size.Y), &Value, ValueMin, ValueMax, TCHAR_TO_ANSI(*Format), RMAImGui::Library::Enum::_ImGuiSliderFlags_::Make(Flags));
#endif
		return false;
	};

	//Tree Node
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook", meta = (AutoCreateRefTerm = "Flags"))
		bool TreeNode(const FString& Label, const TArray<FName> Flags)
	{
#if RMAIMGUI_LIBRARY
		return ImGui::TreeNodeEx(TCHAR_TO_ANSI(*Label), RMAImGui::Library::Enum::_ImGuiTreeNodeFlags_::Make(Flags));
#endif
		return false;
	};

	//Tree Pop
	UFUNCTION(BlueprintCallable, Category = "RMAImGui|Hook")
		void TreePop() { ImGui::TreePop(); };

};
