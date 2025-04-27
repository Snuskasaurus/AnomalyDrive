// Copyright 2019 Kryoshift Software. All Rights Reserved.


#include "DemoFunctions.h"

FString UDemoFunctions::GetProjectVersion()
{
    FString ProjectVersion;
    GConfig->GetString(
        TEXT("/Script/EngineSettings.GeneralProjectSettings"),
        TEXT("ProjectVersion"),
        ProjectVersion,
        GGameIni
        );
    return ProjectVersion;
}