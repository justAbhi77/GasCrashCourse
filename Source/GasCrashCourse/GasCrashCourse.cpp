// Copyright Epic Games, Inc. All Rights Reserved.

#include "GasCrashCourse.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, GasCrashCourse, "GasCrashCourse" );

namespace GasCrashTags
{
	// Define the actual value of the "Player" tag.
	const FName Player = FName("Player");
}
