// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "ProjectSWFCharacter.generated.h"

class UTextRenderComponent;
class UStatusComponent;
class AHitBoxActor;

/**
 * This class is the default character for ProjectSWF, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class AProjectSWFCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;
protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	// ... To falling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* StopJumpAnimation;

	// ... To Jump
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* WhileJumpAnimation;

	// ... To attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* BasicAttackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* DodgeAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* DiedAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		class UPaperFlipbook* DiedLoopAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	// Attacking Actions
	void BasicAttacking();

	// Respawn Player
	void Revive();

	// This also update collision box position
	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	AProjectSWFCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, Category = "Damage")
		void TakeDamage(int32 Damage);

	UFUNCTION(BlueprintCallable, Category = "Player")
		void ResetLastLevel(FName Level);

	FVector ReturnPlayerForce();

	bool DiedOrNot();

private:
	//------- Variables ---------

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool Attacking = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool Dodging = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 NumDodge = 1;

	UPROPERTY(EditAnywhere, Category = "Setup")
		TSubclassOf<AHitBoxActor> HitBoxBluePrint;

	UPROPERTY(EditAnywhere, Category = "Setup")
		int32 MaxNumDodge = 1;

	float CountSeconds = 0;

	bool Dying = false;
	bool TotallyDied = false;

	UPROPERTY(EditAnywhere, Category = "Setup")
		FVector PlayerForce = { 10, 0, 0 };

	FName LastLevel = "DevTest_Level";

	UStatusComponent* Status = nullptr;

	//------- Functions ---------
	// Generate the hit box
	UFUNCTION(BlueprintCallable, Category = "Hit")
		void SpawnHitBox(int32 Damage, float Time, FVector Location, FRotator Rotation, float CapsuleHight, float CapsuleRadius);

	// Attach Status from the bluepirnt
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void AttachStatus(UStatusComponent* NewStatus);

	//------- Basic Attack Setup ---------
	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackActivateTime = 0.26;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackRadius = 39.1;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		float BasicAttackHight = 84.6;

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		FVector BasicAttackSpawnLocation = { 56.69, 0, -11 };

	UPROPERTY(EditAnywhere, Category = "BasicAttack")
		FRotator BasicAttackSpawnRotation = { 0, -90, 0 };

	UFUNCTION(BlueprintCallable, Category = "BasicAttack")
		void SpawnBasicAttack();
};
