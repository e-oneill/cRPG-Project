// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPG, Log, All);

static TAutoConsoleVariable<bool> CVarDebugAll(
	TEXT("rpg.debug.all"),
	false,
	TEXT("Toggles display of all game-specific debug messages.\n")
	TEXT(" false: off\n")
	TEXT(" true: on")
);

static TAutoConsoleVariable<bool> CVarDebugGameState(
	TEXT("rpg.debug.gamestate"),
	false,
	TEXT("Toggles display of all game-state debug messages.\n")
	TEXT(" false: off\n")
	TEXT(" true: on")
);

static TAutoConsoleVariable<bool> CVarDebugNetworking(
	TEXT("rpg.debug.networking"),
	false,
	TEXT("Toggles display of all networking debug messages.\n")
	TEXT(" false: off\n")
	TEXT(" true: on")
);

static TAutoConsoleVariable<bool> CVarDebugActionSystem(
	TEXT("rpg.debug.actions"),
	false,
	TEXT("Toggles display of all action system debug messages.\n")
	TEXT(" false: off\n")
	TEXT(" true: on")
);