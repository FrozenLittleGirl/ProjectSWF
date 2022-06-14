// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectSWFGameMode.h"
#include "ProjectSWFCharacter.h"

AProjectSWFGameMode::AProjectSWFGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = AProjectSWFCharacter::StaticClass();	
}
