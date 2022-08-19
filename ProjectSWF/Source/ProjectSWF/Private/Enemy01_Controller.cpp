// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/Enemy02.h"
#include "Enemy01_Controller.h"

void AEnemy01_Controller::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemy01_Controller::WalkTowardsPlayer() {
	UE_LOG(LogTemp, Warning, TEXT("Player Spotted"));
}

bool AEnemy01_Controller::ReturnPlayerSpotted() {
	return PlayerSpotted;
}