// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"

class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTSWF_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Check if the current pawn is hit by other
	bool CheckBeingAttacked(UCapsuleComponent* MyCollision);

	// Setup values for health etc
	void SetupVariables(bool IsAPlayer, int32 Health, int32 BasicDamage);

private:
	bool IsAPlayer = true;
	int32 Health = 10;
	int32 BasicDamage = 1;
	bool Died = false;

	// Functions

		
};
