// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy02.h"
#include "Enemy03.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSWF_API AEnemy03 : public AEnemy02
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TakeDamage(int32 Damage, int32 ForceDirection, FVector Force) override;

private:
	// ------ Variables ------
	bool NeedToTurn = false;

	// ------ Function ------
	//  reset enemy direction according to Direction
	void UpdateFaceDirection();

	// return 1 if Ploayer is on the right of the enemy, and -1 otherwise
	int32 ReturnPlayerRelativeLocation();

	virtual void BasicAttack() override;
	
};
