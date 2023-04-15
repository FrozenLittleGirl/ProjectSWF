// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHitComponentManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHitComponentManager, Log, All);

class USHitComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEWPROJECT_API USHitComponentManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHitComponentManager();

protected:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool ShowDebugDisplay;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TArray<FName> InvalidTargetGroup;

	UFUNCTION()
	void AttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Called when the game starts
	virtual void BeginPlay() override;
};
