// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy02.generated.h"

class UStatusComponent;
class AHitBoxActor;
class AProjectile;

UCLASS()
class PROJECTSWF_API AEnemy02 : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy02();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakeDamage(int32 Damage, int32 ForceDirection, FVector Force);

protected:
	//------- General -------
	FTimerHandle AttackTimeHandle;

	UPROPERTY(EditAnywhere, Category = "Setup")
		float WalkVelocity = 1;

	UPROPERTY(EditAnywhere, Category = "Setup")
		int32 Direction = 1;

	bool PlayerSpotted = false;

	UStatusComponent* Status = nullptr;

	// Halt

	UPROPERTY(EditAnywhere, Category = "Setup")
		float TurnHaltTime = 2;

	float LastHalt = 0;
	float IndicatedHaltTime = 0;

	// Last Attack Time
	float LastAttack = -BasicAttackInterval;

	bool Halt = false;

	// Attack Player
	UPROPERTY(EditAnywhere, Category = "Setup")
		float DistancePlayer = 140;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AHitBoxActor> HitBoxBluePrint;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AProjectile> ProjectileBluePrint;

	bool Attacking = false;
	float AttakingTime = 0;

	UPROPERTY(EditAnywhere, Category = "Setup")
		float BasicAttackActivateTime = 0.26;

	// Generate the hit box
	virtual void SpawnHitBox(TSubclassOf<AHitBoxActor> Blueprint);

	virtual void SpawnProjectile(TSubclassOf<AProjectile> Blueprint);

	//------- Basic Actions ---------
	UFUNCTION(BlueprintCallable, Category = "Player")
		bool ReturnPlayerSpotted();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void ReverseDirection();

	// Make enemy stop
	UFUNCTION(BlueprintCallable, Category = "Action")
		void MakeHalt(float Seconds);

	UFUNCTION(BlueprintCallable, Category = "Action")
		void PlayerDetected();

	UFUNCTION(BlueprintCallable, Category = "Action")
		void LoseTrackOfPlayer();

	void Walk();

	virtual void BasicAttack();

	//------- Functions ---------
	// Attach Status from the bluepirnt
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachStatus(UStatusComponent* NewStatus);

	// Basic Attack Setup
	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackInterval = 2;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float PreActiveBasicAttack = 0.4;
};
