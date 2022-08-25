// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy02.generated.h"

class UStatusComponent;
class AHitBoxActor;

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
		float HaltTime = 2;

	float LastHalt = 0;
	float IndicatedHaltTime = 0;

	bool Halt = false;

	// Attack Player
	UPROPERTY(EditAnywhere, Category = "Setup")
		float DistancePlayer = 140;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AHitBoxActor> HitBoxBluePrint;

	bool Attacking = false;
	float LastAttack = 0;
	float AttakingTime = 0;

	// Generate the hit box
	UFUNCTION(BlueprintCallable, Category = "Hit")
		void SpawnHitBox(TSubclassOf<AHitBoxActor> Blueprint);

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

	UFUNCTION(BlueprintCallable, Category = "Attack")
		void BasicAttack();

	//------- Functions ---------
	// Attach Status from the bluepirnt
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachStatus(UStatusComponent* NewStatus);

	// Basic Attack Setup
	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackInterval = 4;
};
