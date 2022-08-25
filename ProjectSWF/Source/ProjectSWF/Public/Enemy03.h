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

private:
	void UpdateFaceDirection();
	
};
