// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy03.h"
#include "../ProjectSWFCharacter.h"

// Called every frame
void AEnemy03::Tick(float DeltaTime)
{
	if (PlayerSpotted) {
		UpdateFaceDirection();
	}
	
}

void AEnemy03::UpdateFaceDirection() {
	auto Player = Cast<AProjectSWFCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto PlayerLocation = Player->GetTargetLocation();
	int32 NewDirection = 1;
	if (PlayerLocation.X - GetTargetLocation().X < 0) {
		NewDirection = -1;
	}
	if (Direction != NewDirection) {
		if (NewDirection == 1) {
			SetActorRotation(FRotator{ 0,0,0 });
		}
		else {
			SetActorRotation(FRotator{ 0,180,0 });
		}
		Direction = NewDirection;
	}
}