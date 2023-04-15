// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "SHitComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup="Collision", meta=(DisplayName="HitBoxComponent", BlueprintSpawnableComponent))
class NEWPROJECT_API USHitComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	USHitComponent();
	
	void EnableCollision();
	void DisableCollision();

	void ApplyAttackEffects(AActor* OtherActor, UPrimitiveComponent* OtherComp);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TEnumAsByte<ECollisionEnabled::Type> BasicAttackCollisionType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage;
	
};
