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
#include "Interfaces/IHttpResponse.h"

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

	StatusComp = CreateDefaultSubobject<UStatusComponent>("StatusComp");

	PlayerEState = EState::EState_Idle;
	DodgeLaunchVelocity = FVector(2000.0f, 0.0f, 0.0f);
	DodgeDelay = 0.5f;
	DodgeHalt = 0.1f;
	BasicAttackDelay = 0.33f;
	bDodgeHalt = false;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void AProjectSWFCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
	const float PlayerVelocityZ = PlayerVelocity.Z;

	UPaperFlipbook* DesiredAnimation;
	
	switch (PlayerEState)
	{
	case EState::EState_Dodge:
		DesiredAnimation = DodgeAnimation;
		break;
	case EState::EState_BasicAttack:
		DesiredAnimation = BasicAttackAnimation;
		break;
	default:
		// Are we Jumping?
		if (PlayerVelocityZ > 0.0f)
		{
			DesiredAnimation = WhileJumpAnimation;
		}
		else if (PlayerVelocityZ < 0.0f)
		{
			DesiredAnimation = StopJumpAnimation;
		} 
		// Are we moving or standing still?
		else if (PlayerSpeedSqr > 0.0f)
		{
			DesiredAnimation = RunningAnimation;
		}
		else
		{
			DesiredAnimation = IdleAnimation;
		}
	}

	if (GetSprite()->GetFlipbook() != DesiredAnimation)
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
	if (ensure(StatusComp))
	{
		if (StatusComp->DiedOrNot()) {
			auto Animation = DiedAnimation;
			if ((FPlatformTime::Seconds() - StatusComp->ReturnDyingCount()) >= PlayDeathTime) {
				TotallyDied = true;
				Animation = DiedLoopAnimation;
			}

			if (GetSprite()->GetFlipbook() != Animation)
			{
				GetSprite()->SetFlipbook(Animation);
			}
			return;
		}
	}

	// other
	if (NumDodge == 0) {
		if (GetCharacterMovement()->IsFalling() == false) {
			NumDodge = MaxNumDodge;
		}
	}

	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();
	const float PlayerVelocityZ = PlayerVelocity.Z;
	
	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void AProjectSWFCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("BasicAttack", IE_Pressed, this, &AProjectSWFCharacter::BasicAttack);
	PlayerInputComponent->BindAction("Revive", IE_Pressed, this, &AProjectSWFCharacter::Revive);
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &AProjectSWFCharacter::Dodge);
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

void AProjectSWFCharacter::SpawnHitBox(TSubclassOf<AHitBoxActor> Blueprint) {
	FRotator ActorRotation = FRotator{ 0,0,0 };
	if (GetActorRotation().Yaw != 0) {
		ActorRotation = FRotator{ 0,180,0 };
	}
	auto HitBox = GetWorld()->SpawnActor<AHitBoxActor>(
		Blueprint,
		GetTargetLocation(),
		ActorRotation
	);

	HitBox->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void AProjectSWFCharacter::SpawnBasicAttack() {
	SpawnHitBox(HitBoxBluePrint);
}

void AProjectSWFCharacter::TakeDamageNoDirection(int32 Damage) {
	UE_LOG(LogTemp, Warning, TEXT("%s is taking damage: %d"), *(GetName()), Damage);
	StatusComp->TakeDamage(Damage);
}

void AProjectSWFCharacter::TakeDamage(int32 Damage, int32 ForceDirection, FVector Force) {
	//UE_LOG(LogTemp, Warning, TEXT("%s is taking damage: %d"), *(GetName()), Damage);
	StatusComp->TakeDamage(Damage);
	FVector NewForce = Force;
	if (ForceDirection < 0) {
		NewForce.X = -NewForce.X;
	}
	LaunchCharacter(NewForce, true, false);
}

void AProjectSWFCharacter::ResetLastLevel(FName Level) {
	LastLevel = Level;
}

FVector AProjectSWFCharacter::ReturnPlayerForce() {
	return PlayerForce;
}

bool AProjectSWFCharacter::DiedOrNot() {
	return StatusComp->DiedOrNot();
}

void AProjectSWFCharacter::Dodge()
{
	if (PlayerEState == EState::EState_Dodge || bDodgeHalt)
	{
		return;
	}
	if (GetMovementComponent()->IsFalling())
	{
		if (NumDodge == 1)
		{
			NumDodge = 0;
		} else
		{
			return;
		}
	}
	BasicAttackAbort();
	PlayerEState = EState::EState_Dodge;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);
	
	JumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	GroundFriction = GetCharacterMovement()->GroundFriction;
	Gravity = GetCharacterMovement()->GravityScale;
	GetCharacterMovement()->JumpZVelocity = 0.f;
	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->GroundFriction = 0.f;
	
	LaunchCharacter(DodgeLaunchVelocity * AttackDirection(), true, true);
	GetWorldTimerManager().SetTimer(TimerHandle_Dodge, this, &AProjectSWFCharacter::Dodge_TimeElapsed, DodgeDelay);
}

void AProjectSWFCharacter::Dodge_TimeElapsed()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);
	PlayerEState = EState::EState_Idle;
	GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
	GetCharacterMovement()->GravityScale = Gravity;
	GetCharacterMovement()->GroundFriction = GroundFriction;
	bDodgeHalt = true;
	GetWorldTimerManager().SetTimer(TimerHandle_DodgeHalt, this, &AProjectSWFCharacter::DodgeHalt_TimeElapsed, DodgeHalt);
}

void AProjectSWFCharacter::DodgeHalt_TimeElapsed()
{
	bDodgeHalt = false;
}

void AProjectSWFCharacter::BasicAttack()
{
	if (PlayerEState == EState::EState_Dodge || PlayerEState == EState::EState_BasicAttack)
	{
		return;
	}
	PlayerEState = EState::EState_BasicAttack;
	SpawnHitBox(HitBoxBluePrint);
	GetWorldTimerManager().SetTimer(TimerHandle_BasicAttack, this, &AProjectSWFCharacter::BasicAttack_TimeElapsed, BasicAttackDelay);
}

void AProjectSWFCharacter::BasicAttack_TimeElapsed()
{
	PlayerEState = EState::EState_Idle;
}

void AProjectSWFCharacter::BasicAttackAbort()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BasicAttack);
}

int32 AProjectSWFCharacter::AttackDirection() {
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