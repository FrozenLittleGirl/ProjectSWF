// Copyright Epic Games, Inc. All Rights Reserved.

#include "NewProjectGameMode.h"
#include "NewProjectCharacter.h"

ANewProjectGameMode::ANewProjectGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ANewProjectCharacter::StaticClass();	
}
