// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectSWFCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Public/StatusComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Public/HitBoxActor.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AProjectSWFCharacter

AProjectSWFCharacter::AProjectSWFCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void AProjectSWFCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
	const float PlayerVelocityZ = PlayerVelocity.Z;

	UPaperFlipbook* DesiredAnimation;

	// Deal with Dodging
	if (Dodging) {
		DesiredAnimation = DodgeAnimation;

		if (GetSprite()->GetFlipbook() != DesiredAnimation)
		{
			GetSprite()->SetFlipbook(DesiredAnimation);
		}
		return;
	}

	// Deal with attacking
	if (Attacking) {
		DesiredAnimation = BasicAttackAnimation;

		if (GetSprite()->GetFlipbook() != DesiredAnimation)
		{
			GetSprite()->SetFlipbook(DesiredAnimation);
		}
		
		return;
	}

	// Are we Jumping?
	if (PlayerVelocityZ > 0.0f) {
		DesiredAnimation = WhileJumpAnimation;
	}
	else if (PlayerVelocityZ < 0.0f) {
		DesiredAnimation = StopJumpAnimation;
	} 
	// Are we moving or standing still?
	else if (PlayerSpeedSqr > 0.0f) {
		DesiredAnimation = RunningAnimation;
	}
	else {
		DesiredAnimation = IdleAnimation;
	}

	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AProjectSWFCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// totally died
	if (TotallyDied) { return; }

	// update death animation
	if (Status->DiedOrNot()) {
		auto Animation = DiedAnimation;
		if ((FPlatformTime::Seconds() - Status->ReturnDyingCount()) >= 0.34) {
			TotallyDied = true;
			Animation = DiedLoopAnimation;
		}

		if (GetSprite()->GetFlipbook() != Animation)
		{
			GetSprite()->SetFlipbook(Animation);
		}
		return;
	}

	// other
	if (NumDodge == 0) {
		if (GetCharacterMovement()->IsFalling() == false) {
			NumDodge = MaxNumDodge;
		}
	}

	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AProjectSWFCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &AProjectSWFCharacter::BasicAttacking);
	PlayerInputComponent->BindAction("Revive", IE_Pressed, this, &AProjectSWFCharacter::Revive);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectSWFCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &AProjectSWFCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AProjectSWFCharacter::TouchStopped);
}

void AProjectSWFCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AProjectSWFCharacter::BasicAttacking() {
	

}

void AProjectSWFCharacter::Revive() {
	if (TotallyDied) {
		UGameplayStatics::OpenLevel(GetWorld(), LastLevel);
	}
}

void AProjectSWFCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (Dodging) { return; }

	// Jump on any touch
	Jump();
}

void AProjectSWFCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (Dodging) { return; }

	// Cease jumping once touch stopped
	StopJumping();
}

void AProjectSWFCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Attacking && !Dodging) {
		return;
	}
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

void AProjectSWFCharacter::SpawnHitBox(int32 Damage, float Time, FVector Location, FRotator Rotation, float CapsuleHight, float CapsuleRadius, int32 Direction, FVector Force) {
	FRotator ActorRotation = FRotator{ 0,0,0 };
	if (GetActorRotation().Yaw != 0) {
		ActorRotation = FRotator{ 0,180,0 };
	}
	auto HitBox = GetWorld()->SpawnActor<AHitBoxActor>(
		HitBoxBluePrint,
		GetTargetLocation(),
		ActorRotation
	);

	HitBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	HitBox->StoreValues(Damage, Time);
	HitBox->InitializeHitBox(Location, Rotation, CapsuleHight, CapsuleRadius);
	HitBox->InitializeValues(Direction, Force);
}

void AProjectSWFCharacter::SpawnBasicAttack(int32 Direction) {
	SpawnHitBox(Status->ReturnBasicDamage(), BasicAttackActivateTime, BasicAttackSpawnLocation,
		BasicAttackSpawnRotation, BasicAttackHight, BasicAttackRadius, Direction, PlayerForce);
}

void AProjectSWFCharacter::AttachStatus(UStatusComponent* NewStatus) {
	Status = NewStatus;
}

void AProjectSWFCharacter::TakeDamage(int32 Damage) {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage: %d"), *(GetName()), Damage);
	Status->TakeDamage(Damage);
}

void AProjectSWFCharacter::ResetLastLevel(FName Level) {
	LastLevel = Level;
}

FVector AProjectSWFCharacter::ReturnPlayerForce() {
	return PlayerForce;
}

bool AProjectSWFCharacter::DiedOrNot() {
	return Status->DiedOrNot();
}

int32 AProjectSWFCharacter::FaceDirection() {
	int32 Direction;
	int32 XDirection = GetCharacterMovement()->GetLastUpdateVelocity().X;
	if (XDirection < 0) {
		Direction = -1;
	}
	else if (XDirection > 0) {
		Direction = 1;
	}
	else if (GetControlRotation().Yaw > 90) {
		Direction = -1;
	}
	else {
		Direction = 1;
	}
	return Direction;
}