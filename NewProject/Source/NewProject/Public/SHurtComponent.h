// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "SHurtComponent.generated.h"

class USStatusComponent;

/**
 * 
 */
UCLASS(ClassGroup="Collision", meta=(DisplayName="HurtBoxComponent", BlueprintSpawnableComponent))
class NEWPROJECT_API USHurtComponent : public UCapsuleComponent
{
	GENERATED_BODY()

public:
	USHurtComponent();

	virtual void BeginPlay() override;

	void TakeDamage(AActor* Instigator, float Damage);

private:
	USStatusComponent* OwnerStatusComp;
	
};
