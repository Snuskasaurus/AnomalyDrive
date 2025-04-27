// Copyright 2017-2025 Rafael Marques Almeida. All Rights Reserved.
#include "RMAImGuiUtility.h"
#include "RMAImGuiHook.h"
#include "Interfaces/IPluginManager.h"

FName URMAImGuiFunctionLibrary::GetVersion()
{
	return *IPluginManager::Get().FindPlugin("RMAImGui")->GetDescriptor().VersionName;
}

FName URMAImGuiFunctionLibrary::GetVersionImGui()
{
	return FName(IMGUI_VERSION);
}

FName URMAImGuiFunctionLibrary::GetVersionImPlot()
{
	return FName(IMPLOT_VERSION);
}

FName URMAImGuiFunctionLibrary::GetVersionImSpinner()
{
	return FName(IMSPINNER_VERSION);
}

bool URMAImGuiFunctionLibrary::AddHook(URMAImGuiHook* InHook, const FName InContext)
{
#if RMAIMGUI_LIBRARY
	if (InHook)
	{
		TWeakPtr<SRMAImGuiContext> LContext = FRMAImGui::Get().FindContext(InContext);
		if (!LContext.IsValid())
		{
			auto LSlot = RMAImGui::Slots::GetSlot();
			if (LSlot.IsValid())
			{
				LContext = FRMAImGui::Get().CreateContext(InContext, LSlot.Pin().ToSharedRef());
			}
		}
		if (LContext.IsValid())
		{
			LContext.Pin()->AddHook(InHook);
			return true;
		}
	}
#endif
	return false;
}

bool URMAImGuiFunctionLibrary::RemoveHook(URMAImGuiHook* InHook, const FName InContext)
{
#if RMAIMGUI_LIBRARY
	if (InHook)
	{
		TWeakPtr<SRMAImGuiContext> LContext = FRMAImGui::Get().FindContext(InContext);
		if (LContext.IsValid())
		{
			LContext.Pin()->RemoveHook(InHook);
			return true;
		}
	}
#endif
	return false;
}

bool URMAImGuiFunctionLibrary::FlushHooks(const FName InContext)
{
#if RMAIMGUI_LIBRARY
	TWeakPtr<SRMAImGuiContext> LContext = FRMAImGui::Get().FindContext(InContext);
	if (LContext.IsValid())
	{
		LContext.Pin()->FlushHooks();
		return true;
	}
#endif
	return false;
}
