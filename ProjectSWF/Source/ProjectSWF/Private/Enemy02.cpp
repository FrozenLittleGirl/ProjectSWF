// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy02.h"
#include "../Public/StatusComponent.h"
#include "../ProjectSWFCharacter.h"
#include "../Public/HitBoxActor.h"
#include "../Public/Projectile.h"
#include <Runtime/Engine/Classes/Kismet/KismetMathLibrary.h>

// Sets default values
AEnemy02::AEnemy02()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//GetWorld()->GetTimerManager().SetTimer(AttackTimeHandle, this, &AEnemy02::BasicAttack, BasicAttackInterval, true, BasicAttackInterval);
}

// Called when the game starts or when spawned
void AEnemy02::BeginPlay()
{
	Super::BeginPlay();
	
	if (Direction == -1) {
		bool result = SetActorRotation({ 0, 180, 0 });
	}
}

// Called every frame
void AEnemy02::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		BasicAttack();
		Attacking = false;
		LastAttack = FPlatformTime::Seconds() + BasicAttackActivateTime;
		MakeHalt(BasicAttackActivateTime);
		return;
	}

	if (PlayerSpotted) {
		float interval = Player->GetTargetLocation().X - GetTargetLocation().X;
		if (interval < 0) {
			Direction = -1;
		}
		else {
			Direction = 1;
		}

		// Try to attack or approach player
		if (DistancePlayer < abs(interval)) {
			Walk();
		}
		else {
			if (FPlatformTime::Seconds() - LastAttack >= BasicAttackInterval) {
				Attacking = true;
				MakeHalt(PreActiveBasicAttack);
			}
		}
	}
	else {
		Walk();
	}
}

// Called to bind functionality to input
void AEnemy02::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy02::TakeDamage(int32 Damage, int32 ForceDirection, FVector Force) {
	Status->TakeDamage(Damage);
	FVector NewForce = {Force.X * ForceDirection, Force.Y, Force.Z};
	LaunchCharacter(NewForce, true, false);
}

void AEnemy02::TakeDamageNoDirection(int32 Damage) {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage of %d"), *(GetName()), Damage);
	Status->TakeDamage(Damage);
}

void AEnemy02::Walk() {	
	// Enemy will not halt if player is spotted
	Attacking = false;

	this->AddMovementInput(FVector{ 1,0,0 }, Direction * WalkVelocity);
	if (Direction >= 0) {
		this->SetActorRotation(FRotator(0, 0, 0));
	}
	else {
		this->SetActorRotation(FRotator(0, 180, 0));
	}
}

void AEnemy02::ReverseDirection() {
	MakeHalt(TurnHaltTime);
	Direction = Direction * -1;
}

void AEnemy02::MakeHalt(float Seconds) {
	Halt = true;
	IndicatedHaltTime = Seconds;
	LastHalt = FPlatformTime::Seconds();
}

bool AEnemy02::ReturnPlayerSpotted() {
	return PlayerSpotted;
}

void AEnemy02::PlayerDetected() {
	PlayerSpotted = true;
	Halt = false;
	UE_LOG(LogTemp, Warning, TEXT("%s: Player Spotted!"), *(GetName()));
}

void AEnemy02::LoseTrackOfPlayer() {
	UE_LOG(LogTemp, Warning, TEXT("%s: Lose Track of Player!"), *(GetName()));
	PlayerSpotted = false;
	MakeHalt(TurnHaltTime);
}

void AEnemy02::AttachStatus(UStatusComponent* NewStatus) {
	Status = NewStatus;
}

void AEnemy02::SpawnHitBox(TSubclassOf<AHitBoxActor> Blueprint) {
	FRotator ActorRotation = FRotator{ 0,0,0 };
	if (GetActorRotation().Yaw != 0) {
		ActorRotation = FRotator{ 0,180,0 };
	}
	auto HitBox = GetWorld()->SpawnActor<AHitBoxActor>(
		Blueprint,
		GetTargetLocation(),
		ActorRotation
	);

	HitBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AEnemy02::SpawnProjectile(TSubclassOf<AProjectile> Blueprint, float Length) {
	auto Player = Cast<AProjectSWFCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	auto ProjectVector = Player->GetTargetLocation() - GetTargetLocation();
	ProjectVector.Y = 0;
	auto result = ProjectVector.Normalize();
	float CosAngle = FVector::DotProduct(ProjectVector, FVector{ 1,0,0 }) / ProjectVector.Size();
	float Angle = FMath::RadiansToDegrees(acos(CosAngle));

	FRotator Vec = { 0,0,0 };
	if (Player->GetTargetLocation().Z - GetTargetLocation().Z < 0) {
		Vec.Pitch = -Angle;
	}
	else {
		Vec.Pitch = Angle;
	}

	auto Projectile = GetWorld()->SpawnActor<AProjectile>(
		Blueprint,
		GetTargetLocation() + ProjectVector * Length,
		Vec
	);
	// ProjectVector here seems to be useless
	Projectile->LaunchProjectile(ProjectVector);
}

void AEnemy02::BasicAttack() {


	SpawnHitBox(HitBoxBluePrint);
	AttakingTime = FPlatformTime::Seconds();
}