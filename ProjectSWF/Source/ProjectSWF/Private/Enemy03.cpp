// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy03.h"
#include "../Public/StatusComponent.h"
#include "../ProjectSWFCharacter.h"

// Called every frame
void AEnemy03::Tick(float DeltaTime)
{
	if (Status->DiedOrNot()) {
		bool result = Destroy();
		return;
	}

	auto Player = Cast<AProjectSWFCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player->DiedOrNot()) { return; }

	if (Halt) {
		if (FPlatformTime::Seconds() - LastHalt >= IndicatedHaltTime) {
			Halt = false;
		}
		return;
	}

	if (Attacking) {
		UE_LOG(LogTemp, Warning, TEXT("Spawning Projectile"));
		BasicAttack();
		Attacking = false;
		MakeHalt(BasicAttackActivateTime);
	}

	if (NeedToTurn) {
		Direction *= -1;
		UpdateFaceDirection();
		NeedToTurn = false;
	}

	if (PlayerSpotted) {
		int32 PlayerRelativeLocation = ReturnPlayerRelativeLocation();
		if (PlayerRelativeLocation != Direction) {
			// Give time to turn to anothe direction
			MakeHalt(TurnHaltTime);
			NeedToTurn = true;
		}
		Attacking = true;
		MakeHalt(BasicAttackInterval);
	}
	
}

void AEnemy03::UpdateFaceDirection() {
	bool result = true;
	if (Direction == 1) {
		result = SetActorRotation({ 0, 0, 0 });
	}
	else {
		result = SetActorRotation({ 0, 180, 0 });
	}
}

int32 AEnemy03::ReturnPlayerRelativeLocation() {
	int32 PlayerDirection = 1;
	auto Player = Cast<AProjectSWFCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto PlayerLocation = Player->GetTargetLocation();
	if (PlayerLocation.X - GetTargetLocation().X < 0) {
		PlayerDirection = -1;
	}
	return PlayerDirection;
}

void AEnemy03::BasicAttack() {
	SpawnProjectile(ProjectileBluePrint, ProjectileRelativeLength);
	AttakingTime = FPlatformTime::Seconds();
}

void AEnemy03::TakeDamage(int32 Damage, int32 ForceDirection, FVector Force) {
	if (ForceDirection == Direction) {
		UE_LOG(LogTemp, Warning, TEXT("Takeing Damage"));
		Status->TakeDamage(Damage);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Attack Blocked"));
	}
}