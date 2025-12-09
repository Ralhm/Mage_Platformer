// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "DrawDebugHelpers.h"

#include "Engine/Texture.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GrapplePoint.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "PhysicsEngine/RadialForceComponent.h"


//---Additive Movement
//Skidding builds up heat, allows you to carry momentum and turn on a dime
// just a few attacks that double as movement options
// jump out of dash that carries momentum
// for queueing up moves when you add grapples, just check if you're in a grapple state or not and then decide which type of the queued action to execute
// 
// Make tug a montage?
// 
// 
// --Electric Ball Mode
// Landing on a slop should not immediately send you in the direction of the slope (Like on that one curve)
// 
// when grappling, prioritize enemies that are closer?
//https://x.com/log64dev/status/1805094711142863203


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = NULL;

	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);

	Outline = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Outline"));
	Outline->SetupAttachment(GetMesh());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraAttachmentArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());



	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ActualCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	Staff = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Staff"));
	Staff->SetupAttachment(GetMesh());

	StaffOutline = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffOutline"));
	StaffOutline->SetupAttachment(Staff);

	ManaComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ManaParticleComp"));
	ManaComp->SetupAttachment(Staff);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::StartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	PlayerInputComponent->BindAction("Dive", IE_Pressed, this, &APlayerCharacter::DiveEnter);

	PlayerInputComponent->BindAction("FireAction", IE_Pressed, this, &APlayerCharacter::FireAction);
	PlayerInputComponent->BindAction("IceAction", IE_Pressed, this, &APlayerCharacter::IceAction);
	PlayerInputComponent->BindAction("ElectricAction", IE_Pressed, this, &APlayerCharacter::ElecAction);

	PlayerInputComponent->BindAction("GrappleEnemy", IE_Pressed, this, &APlayerCharacter::GrappleEnemy);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

	TeamId = FGenericTeamId(1);
}



// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BaseGravity = GetCharacterMovement()->GravityScale;
	BaseAirControl = GetCharacterMovement()->AirControl;
	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	baseJumpVelocity = GetCharacterMovement()->JumpZVelocity;
	BaseRotationRate = GetCharacterMovement()->RotationRate;
	BaseLateralFriction = GetCharacterMovement()->FallingLateralFriction;
	BaseBrakingDecelerationWalking = GetCharacterMovement()->BrakingDecelerationWalking;
	BaseBrakingDecelerationFalling = GetCharacterMovement()->BrakingDecelerationFalling;
	BaseBrakingFrictionFactor = GetCharacterMovement()->BrakingFrictionFactor;
	BaseFriction = GetCharacterMovement()->GroundFriction;

	Staff->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false), "hand_rSocket");

	TimeManager.AttackTimer = AttackCountdownTime;
	TimeManager.InvincibilityTimer = InvincibilityCountdownTime;
	TimeManager.DashTimer = DashCountdownTime;
	TimeManager.HealTimer = HealCountdownTime;
	TimeManager.CoyoteTimer = CoyoteTime;
	TimeManager.BoostTimer = BoostCountdownTime;

	CanDash = true;
	FaceState = Smile;
	UpdateFace(FaceState, true);
	CurrentState = Normal;
	HP = MaxHP;
	Mana = MaxMana;
	HealthPercent = HP / MaxHP;
	CanGrapple = false;
	CanSkid = true;

	DashPercent = TimeManager.DashTimer / DashCountdownTime;
	ManaPercent = Mana / MaxMana;
	DashBoost = false;


	ActorsToIgnore.Add(this);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




}


void APlayerCharacter::MoveForward(float Value) {
	/*
		if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();// find out which way is forward
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // get forward vector
		AddMovementInput(Direction, Value);
	}
	*/

}


void APlayerCharacter::MoveRight(float Value) {
	/*

		if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();// find out which way is right
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);// get right vector
		AddMovementInput(Direction, Value);// add movement in that direction
	}
	*/

}


void APlayerCharacter::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void APlayerCharacter::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	//GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Yellow, "Hit Enemy!");

}






void APlayerCharacter::StartJump() {
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Current JumpCount: %d"), JumpCurrentCount));
	if (CurrentState == Normal || CurrentState == InAir || CurrentState == WallJump || CurrentState == WallClimb) {
		GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Emerald, "Normal Jump");
		GetCharacterMovement()->bNotifyApex = true;
		JumpKeyHoldTime = 0.0f;
		CurrentState = InAir;
		CanSkid = false;
		bPressedJump = true;
		GetCharacterMovement()->GravityScale = jumpingGravity; // Set gravity to lower value when jumping

		if (JumpCurrentCount > 0) {
			GetCharacterMovement()->JumpZVelocity = DoubleJumpPower;
		}
	}
	else if (CurrentState == WallSliding) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "WallJumped!");
		CurrentState = WallJump;
		//GetCharacterMovement()->GravityScale = jumpingGravity;
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->AirControl = WallJumpAirControl;
		LaunchCharacter(((GetActorForwardVector() * WallJumpPower) + (GetActorUpVector() * (WallJumpPower + 50.0))), false, false);
		GetCapsuleComponent()->SetEnableGravity(true);
	}
	else if (CurrentState == TurnAround) {
		GEngine->AddOnScreenDebugMessage(-1, 7.0f, FColor::Magenta, "TurningAround Jump");
		GetCharacterMovement()->bNotifyApex = true;
		JumpKeyHoldTime = 0.0f;
		bPressedJump = true;

		float StoredSpeed = GetCharacterMovement()->Velocity.Size() * SlipSpeedBoost;

		GetWorldTimerManager().ClearTimer(TimeManager.SkidHandle);
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->JumpZVelocity += StoredSpeed;
		GetCharacterMovement()->GravityScale = jumpingGravity;

		CurrentState = InAir;
		AddActorLocalRotation(FRotator(0, 180, 0));
		CanSkid = false;

		//GetCharacterMovement()->RotationRate = FRotator(0,0,0);
		//CurrentState = InAir;
	}
	else if (CurrentState == Dive) {
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->AddImpulse(FVector(0, 0, DashCancelBoostAmount));
		CurrentState = InAir;
		DiveExit();
	}
	else if (CurrentState == Slide) {
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->AddImpulse(FVector(0, 0, SlidecancelBoostAmount));
		IncrementJumpCount();
		CurrentState = InAir;
		DiveExit();
	}
	else if (CurrentState == WallCling) {
		JumpCurrentCount = 1;
		CanDoubleJump = true;
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		CurrentState = WallClimb;
		GetCharacterMovement()->GravityScale = BaseGravity;
		GetCharacterMovement()->AddImpulse(FVector(0, 0, ClimbBoost));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "JUmping out of cling");

	}
	else if (CurrentState == Ice && GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking) {
		FVector LaunchPower = (GetActorUpVector() * IceJumpPower);

		LaunchCharacter(LaunchPower, false, false);
	}
	else if (IceGrappling) {
		IceGrappleEnd();
	}

	/*
	else if (CurrentState == WallRun) {
		GetCharacterMovement()->GravityScale = BaseGravity;
		GetCapsuleComponent()->SetEnableGravity(true);
		GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0, 0, 0));
		IsWallLeftSide = false;
		IsWallRightSide = false;
		OnRightSide = false;
		OnLeftSide = false;
		CurrentState = WallJump;
		LaunchCharacter((((GetActorForwardVector()) + WallNormal * WallRunJumpPower) + (GetActorUpVector() * (WallRunJumpPower + 100.0))), false, false);

	}
	*/
	/*
	if (CanBuffer) {
		JumpBuffer = true;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Buffered");
	}
	*/
}


void APlayerCharacter::StopJump() {
	bPressedJump = false;
	ResetJumpState();
}


void APlayerCharacter::StartSprint() {
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}


void APlayerCharacter::StopSprint() {
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}


void APlayerCharacter::DiveEnter() {
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Dashing!");
	if (CanDash && (CurrentState == Normal || CurrentState == InAir || CurrentState == WallJump)) {
		CurrentState = Dive;
		//GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->AddImpulse((GetActorForwardVector() * DivePower) + (GetActorUpVector() * DashUpPower), true);
		CanDash = false;
		DashBoost = true;

		GetCharacterMovement()->AirControl = DiveAirControl;
		GetCharacterMovement()->FallingLateralFriction = DiveLateralFriction;
		GetCharacterMovement()->GravityScale = DiveGravity;
		GetCharacterMovement()->GroundFriction = DiveFriction;
		GetCharacterMovement()->BrakingDecelerationWalking = DiveBrakingDecelerationWalking;
		GetCharacterMovement()->BrakingFrictionFactor = DiveBrakingFriction;
		GetCharacterMovement()->RotationRate = FRotator(0, 0, 0);


	}
}

void APlayerCharacter::DiveExit() {
	ResetPhysics();
	GetCharacterMovement()->RotationRate = BaseRotationRate;
}


void APlayerCharacter::TurnAroundCheck() {
	if (GetCharacterMovement()->IsFalling() || CurrentState != Normal) {
		return;
	}	

	float Dot = FVector::DotProduct(OldDir, (GetActorForwardVector() * FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0).Size()).GetSafeNormal());
	if (Dot <= -0.6f && CanSkid && FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0).Size() > 100) {
		GetCharacterMovement()->GroundFriction = TurnAroundFriction;
		CurrentState = TurnAround;
		GetWorldTimerManager().SetTimer(TimeManager.SkidHandle, this, &APlayerCharacter::SkidTimer, 0.1f, true);

	}
}

void APlayerCharacter::SkidTimer() {
	if (GetCharacterMovement()->Velocity.Size() < 20.0f) {
		GetWorldTimerManager().ClearTimer(TimeManager.SkidHandle);
		GetCharacterMovement()->GroundFriction = BaseFriction;
		CurrentState = Normal;
	}
}

#pragma region Ice

void APlayerCharacter::IceAction() {
	if ((CurrentState == Electric || CurrentState == Fire) && !ElecRecoilActive) {
		if (InBallMode) {
			ElecBallModeEnd();
			IceDash();
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Queueing Ice!");
			QueueAction(Ice);
			return;
		}

	}
	else if (Tugging) {
		QueueAction(Ice);
		return;
	}

	if (CurrentState == GrapplingEnemy && IsLookTargetEnemy) {
		IceGrappleBegin();
		return;
	}


	if (CanIce && (CurrentState == Normal || CurrentState == InAir || CurrentState == WallJump || ElecRecoilActive)) {
		IceDash();
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Can't Ice Attack!");
	}
}

void APlayerCharacter::IceSlidePhysics() {
	//Don't apply slide physics if we're going UP
	//This Should allow halfpipes and curves to work for Ball Mode
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Purple, FString::Printf(TEXT("Up Dot Product: %f"), FVector::DotProduct(GetActorUpVector(), FVector::UpVector)));
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("Forward Dot Product: %f"), FVector::DotProduct(GetActorForwardVector(), FVector::UpVector)));

	GetCharacterMovement()->AddForce(CalculateFloorInfluence(GetCharacterMovement()->CurrentFloor.HitResult.ImpactNormal) * IceSlideForce);

	//We should only set pitch or yaw or whatever here

	//SetActorRotation(GetCharacterMovement()->Velocity.GetSafeNormal().Rotation());
}


void APlayerCharacter::IceDash() {
	CurrentState = Ice;
	CanIce = false;
	CanIceBounce = true;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "Ice Dashing!");


	FVector Dir;

	if (CurrentHoldDir.IsNearlyZero()) {
		Dir = GetActorForwardVector();
	}
	else {
		Dir = CurrentHoldDir.GetSafeNormal();
	}

	FVector LaunchPower = (Dir * IceHorizontalPower) + (GetActorUpVector() * IceVerticalPower);
	FVector CarryOverVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0).Size() * LaunchPower.GetSafeNormal();

	LaunchCharacter(LaunchPower + CarryOverVelocity, true, true);




	GetCharacterMovement()->AirControl = IceAirControl;
	GetCharacterMovement()->FallingLateralFriction = IceLateralFriction;
	GetCharacterMovement()->GravityScale = IceGravity;
	GetCharacterMovement()->GroundFriction = IceFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = IceBrakingDecelerationWalking;
	GetCharacterMovement()->BrakingDecelerationFalling = IceBrakingDecelerationFalling;
	GetCharacterMovement()->BrakingFrictionFactor = IceBrakingFriction;

	BeginCountDown("Ice");
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Attack!");
}


void APlayerCharacter::IceGrappleBegin() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Grapple!");
	CurrentState = Ice;
	GetWorldTimerManager().ClearTimer(TimeManager.GrappleHandle);

	GetCharacterMovement()->AirControl = IceAirControl;
	GetCharacterMovement()->FallingLateralFriction = IceLateralFriction;
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->GroundFriction = IceFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = IceBrakingDecelerationWalking;
	GetCharacterMovement()->BrakingDecelerationFalling = IceBrakingDecelerationFalling;
	GetCharacterMovement()->BrakingFrictionFactor = IceBrakingFriction;

	IceGrappling = true;

	if (ForwardAxisVal != 0 && GetCharacterMovement()->IsFalling()) {


		if (ForwardAxisVal < 0) {
			IceGrappleDir = 1;
		}
		else {
			IceGrappleDir = -1;
		}
		GetWorldTimerManager().SetTimer(TimeManager.IceHandle, this, &APlayerCharacter::IceGrappleTimerVertical, IceGrappleRate, true);
		return;
	}
	else {
		if (RightAxisVal < 0) {
			IceGrappleDir = -1;
		}
		else {
			IceGrappleDir = 1;
		}
		GetWorldTimerManager().SetTimer(TimeManager.IceHandle, this, &APlayerCharacter::IceGrappleTimerHorizontal, IceGrappleRate, true);
		return; 
	}



}


void APlayerCharacter::IceGrappleForceHorizontal() {
	if (CurrentLookTarget == nullptr) {
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, "Ice Grappling!");

	FVector Dir = FVector::CrossProduct((GetActorLocation() - CurrentLookTarget->GetActorLocation()).GetSafeNormal(), GetActorUpVector() * IceGrappleDir);
	GetCharacterMovement()->Velocity = Dir * IceGrapplePower;

	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Dir * 100, FColor::Magenta, false, 0.1f, 0, 20.0f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), CurrentLookTarget->GetActorLocation(), FColor::Blue, false, 0.1f, 0, 20.0f);
}

void APlayerCharacter::IceGrappleForceVertical() {
	if (CurrentLookTarget == nullptr) {
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, "Ice Grappling!");




	FVector Dir = FVector::CrossProduct((GetActorLocation() - CurrentLookTarget->GetActorLocation()).GetSafeNormal(), GetActorRightVector() * IceGrappleDir);
	GetCharacterMovement()->Velocity = Dir * IceGrapplePower;

	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Dir * 100, FColor::Magenta, false, 0.1f, 0, 20.0f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), CurrentLookTarget->GetActorLocation(), FColor::Blue, false, 0.1f, 0, 20.0f);
}

void APlayerCharacter::IceGrappleEnd(bool Launch) {
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Cyan, "Ice Grapple End!");

	IceGrappling = false;
	if (IceGrappleLaunch && Launch) {
		LaunchCharacter(GetCharacterMovement()->Velocity.GetSafeNormal() * IceGrappleLaunchPower, false, true);
	}

	TimeManager.IceGrappleTimer = 0;
	GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
	ResetPhysics();

	if (GetCharacterMovement()->IsFalling()) {
		CurrentState = InAir;
	}
	else {
		CurrentState = Normal;

	}
}

void APlayerCharacter::IceGrappleTimerHorizontal() {

	TimeManager.IceGrappleTimer += IceGrappleRate;

	IceGrappleForceHorizontal();


	if (TimeManager.IceGrappleTimer > IceGrappleDuration) {
		IceGrappleEnd();

	}

}

void APlayerCharacter::IceGrappleTimerVertical() {
	TimeManager.IceGrappleTimer += IceGrappleRate;
	IceGrappleForceVertical();


	if (TimeManager.IceGrappleTimer > IceGrappleDuration) {
		IceGrappleEnd();

	}




}

void APlayerCharacter::IceBounce() {
	CanIceBounce = false;


	FVector LaunchPower = (GetActorUpVector() * IceBouncePower);
	FVector CarryOverVelocity = GetCharacterMovement()->Velocity.Size() * LaunchPower.GetSafeNormal();

	LaunchCharacter(LaunchPower, true, true);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Bouncing!");
}


void APlayerCharacter::IceEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
	TimeManager.IceGrappleTimer = 0;

	if (CurrentState == WallSliding || CurrentState == WallClimb || CurrentState == Fire || CurrentState == Electric) {
		return;
	}


	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling) {
		CurrentState = InAir;
		ResetPhysics();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Ended in air!");

	}
	else if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking) {
		

		if (CurrentState != Electric && CurrentState != Fire) {

			GetWorldTimerManager().SetTimer(TimeManager.IceHandle, this, &APlayerCharacter::IceFrictionTimer, IceFrictionTime);
		}



		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Ended on ground!");
		CanIce = true;
		CurrentState = Normal;
	}
}

void APlayerCharacter::IceFrictionTimer() {
	GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
	ResetPhysics();
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Ice Friction Ended!");
}

#pragma endregion

#pragma region Fire

void APlayerCharacter::FireAction() {
	if (InBallMode) {
		ElecBallModeEnd();
		FireBounce();
	}
	if ((CurrentState == Electric) && !ElecRecoilActive) {
		QueueAction(Fire);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Queueing Fire!");
		return;
	}
	else if (Tugging) {
		QueueAction(Fire);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Currently Tugging!");
		return;
	}

	if (CurrentState == GrapplingEnemy) {
		GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
		FireGrapple();

		return;
	}

	if (CanFire) {

		if ((CurrentState == Normal || CurrentState == InAir || CurrentState == WallJump || (CurrentState == Ice) || ElecRecoilActive)) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Action!");
			GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
			if (CurrentLookTarget != nullptr && IsLookTargetEnemy && GetCharacterMovement()->IsFalling()) {
				FireDashEnemy();
				return;
			}
			else {
				FireDash();
				return;
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "FAILED TO DO FIRE ACTION!");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "IN BAD STATE!");
		}

		return;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "CAN'T DO FIRE!");
	}


}

void APlayerCharacter::FireDash() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Attack!");
	CurrentState = Fire;
	CanFire = false;

	StopJump();



	FVector Dir;
	if (CurrentHoldDir.IsNearlyZero()) {
		Dir = GetActorForwardVector();
	}
	else {
		Dir = CurrentHoldDir.GetSafeNormal();
	}


	FVector LaunchPower = Dir * FireDashPower;

	FVector CarryOverVelocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0).Size() * Dir * FireCarryOverFactor;

	FVector Final = LaunchPower + CarryOverVelocity;

	GetCharacterMovement()->StopMovementImmediately();

	//GetCharacterMovement()->AddImpulse(LaunchPower + CarryOverVelocity);
	GetCharacterMovement()->Velocity = (LaunchPower + CarryOverVelocity);
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity.GetSafeNormal()));
	//LaunchCharacter(LaunchPower + CarryOverVelocity, true, true);




	GetCharacterMovement()->AirControl = FireAirControl;
	GetCharacterMovement()->FallingLateralFriction = FireLateralFriction;
	GetCharacterMovement()->GravityScale = FireGravity;
	GetCharacterMovement()->GroundFriction = FireFriction;
	GetCharacterMovement()->BrakingDecelerationFalling = FireBrakingDecelerationFalling;
	GetCharacterMovement()->BrakingDecelerationWalking = FireBrakingDecelerationWalking;
	GetCharacterMovement()->BrakingFrictionFactor = FireBrakingFrictionFactor;


	BeginCountDown("Fire");


}

void APlayerCharacter::FireDashEnemy() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Dashing towards enemy!");
	CurrentState = Fire;
	CanFire = false;

	StopJump();

	FVector Dir;
	Dir = (CurrentLookTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal() * 2;


	FVector LaunchPower = Dir * FireDashPower;
	FVector Final = LaunchPower;

	GetCharacterMovement()->StopMovementImmediately();

	//GetCharacterMovement()->AddImpulse(LaunchPower + CarryOverVelocity);
	GetCharacterMovement()->Velocity = (LaunchPower);
	SetActorRotation(UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->Velocity.GetSafeNormal()));
	//LaunchCharacter(LaunchPower + CarryOverVelocity, true, true);




	GetCharacterMovement()->AirControl = FireAirControl;
	GetCharacterMovement()->FallingLateralFriction = FireLateralFriction;
	GetCharacterMovement()->GravityScale = FireGravity;
	GetCharacterMovement()->GroundFriction = FireFriction;
	GetCharacterMovement()->BrakingDecelerationFalling = FireBrakingDecelerationFalling;
	GetCharacterMovement()->BrakingDecelerationWalking = FireBrakingDecelerationWalking;
	GetCharacterMovement()->BrakingFrictionFactor = FireBrakingFrictionFactor;



	BeginCountDown("Fire");
}

void APlayerCharacter::FireGrapple() {
	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, "Fire Grapple!");


	if (GetCharacterMovement()->IsFalling()) {
		if (IsLookTargetEnemy) {
			FireDashEnemy();
		}
		else {
			TugAction();

		}


		return;
	}
	else {
		if (IsLookTargetEnemy) {
			Cast<ACharacter>(CurrentLookTarget)->LaunchCharacter(GetActorUpVector() * FireGrapplePower, true, true);
			CurrentState = Normal;
		}
		else {
			TugAction();
		}
	}
}

void APlayerCharacter::TugAction() {
	Cast<AGrapplePoint>(CurrentLookTarget)->Tug();
	Tugging = true;

	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance) {
		float MontageLength = instance->Montage_Play(TugMontage, TugMontageSpeed);
		Outline->GetAnimInstance()->Montage_Play(TugMontage, TugMontageSpeed);


		GetWorld()->GetTimerManager().SetTimer(TimeManager.AnimationHandle, this, &APlayerCharacter::TugActionEnd, MontageLength);
	}

}

void APlayerCharacter::TugActionEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.AnimationHandle);
	GetWorldTimerManager().ClearTimer(TimeManager.GrappleHandle);
	GetCharacterMovement()->GravityScale = BaseGravity;

	Tugging = false;

	if (GetCharacterMovement()->IsFalling()) {
		CurrentState = InAir;
	}
	else {
		CurrentState = Normal;
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Tug Action Ending!");
	ActivateQueuedAction();
}

void APlayerCharacter::GrappleEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.GrappleHandle);
	GetCharacterMovement()->GravityScale = BaseGravity;

	Tugging = false;

	if (GetCharacterMovement()->IsFalling()) {
		CurrentState = InAir;
	}
	else {
		CurrentState = Normal;
	}

	ActivateQueuedAction();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Grapple Ending!");
}

void APlayerCharacter::FireBounce(float Power) {
	float Speed;
	GetCharacterMovement()->AirControl = FireBounceAirControl;
	CurrentState = Fire;
	if (Power == 0) {
		Speed = GetCharacterMovement()->Velocity.Size() * FireBounceBoost;
	}
	else {
		Speed = Power * FireBounceBoost;
	}



	FVector LaunchPower = (GetActorForwardVector() * Speed) + (GetActorUpVector() * FireBouncePower);
	FVector CarryOverVelocity = GetCharacterMovement()->Velocity.Size() * LaunchPower.GetSafeNormal();

	LaunchCharacter(LaunchPower + CarryOverVelocity, true, true);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Bouncing!");
}


void APlayerCharacter::FireBounceEnemy() {

	LaunchCharacter(GetActorUpVector() * FireBounceEnemyPower, true, true);
	GetCharacterMovement()->AirControl = BaseAirControl;
	CurrentState = InAir;
	CanFire = true;
	CanIce = true;
	CanDoubleJump = true;
	JumpCurrentCount = 1;
}


void APlayerCharacter::FireEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.FireHandle);

	if (CurrentState == WallSliding) {
		return;
	}

	ResetPhysics();


	if (QueuedAction != Normal) {
		ActivateQueuedAction();
		return;
	}





	if (GetCharacterMovement()->IsFalling()) {
		CurrentState = InAir;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Ending in Air!");
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Ending on ground!");
		CanIce = true;
		CanIceBounce = true;
		CurrentState = Normal;
		GetWorldTimerManager().SetTimer(TimeManager.FireHandle, this, &APlayerCharacter::FireDelayTimer, FireDelayTime);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Ending!");
}

void APlayerCharacter::FireEndGrounded() {
	GetWorldTimerManager().ClearTimer(TimeManager.FireHandle);

	ResetPhysics();


	if (QueuedAction != Normal) {
		ActivateQueuedAction();
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Ending on ground!");
	CurrentState = Normal;

	TurnAroundCheck();

	GetWorldTimerManager().SetTimer(TimeManager.FireHandle, this, &APlayerCharacter::FireDelayTimer, FireDelayTime);
}

void APlayerCharacter::FireDelayTimer() {
	GetWorldTimerManager().ClearTimer(TimeManager.FireHandle);
	CanFire = true;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Fire Delay Ending!");
}

#pragma endregion

#pragma region Elec

void APlayerCharacter::ElecAction() {
	if (InBallMode) {
		ElecBallModeEnd();
		return;
	}

	if (CurrentState == GrapplingEnemy && IsLookTargetEnemy) {
		ElecGrapple();
		return;
	}



	if (CanElectric) {
		ElecDash();

	}


}

const FVector APlayerCharacter::CalculateFloorInfluence(const FVector& FloorNormal) {
	//Use VectorUp here, NOT GetActorUpVector
	//This adjusts direction
	FVector NewUpDir = FVector::CrossProduct(FloorNormal, FVector::CrossProduct(FloorNormal, FVector::UpVector)).GetSafeNormal();
	
	//This adjusts scale
	NewUpDir *= (1.0 - FVector::DotProduct(FloorNormal, FVector::UpVector));


	return NewUpDir;
}


const FVector APlayerCharacter::CalculateElecFloorInfluence(const FVector& FloorNormal) {
	//Use VectorUp here, NOT GetActorUpVector
	//This adjusts direction
	FVector NewUpDir = FVector::CrossProduct(FloorNormal, FVector::CrossProduct(FloorNormal, FVector::UpVector)).GetSafeNormal();

	//This adjusts scale
	float Scale = (1.0 - FVector::DotProduct(FloorNormal, FVector::UpVector));
	NewUpDir *= Scale * Scale;
	


	return NewUpDir;
}

void APlayerCharacter::ElecSlidePhysics() {
	//Don't apply slide physics if we're going UP
	//This Should allow halfpipes and curves to work for Ball Mode
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Purple, FString::Printf(TEXT("Up Dot Product: %f"), FVector::DotProduct(GetActorUpVector(), FVector::UpVector)));

	//Consider making this static to save space
	//In order for the halfpipe to work, we'll need to NOT Set Rotation in the direction of the slide
	//Instead, keep rotation set to velocity
	FVector SlideDir = CalculateElecFloorInfluence(BallModeHit.ImpactNormal);
	if (SlideDir.IsNearlyZero()) {
		return;
	}
	//GetCharacterMovement()->AddForce(CalculateFloorInfluence(SlideDir * ElecSlideForce);

	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("UpDown Dot Product: %f"), FVector::DotProduct(GetActorUpVector(), FVector::DownVector)));
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (SlideDir * 1000.0f), FColor::Magenta, false, 0.1f, 0, 20.0f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * 1000.0f), FColor::Green, false, 0.1f, 0, 20.0f);

	//Try scaling slideforce with dot product

	//Try scaling down the floor influence for steep slopes
	
	//Only apply slide force if we're NOT upside down in loop/curve
	if (FVector::DotProduct(GetActorUpVector(), FVector::DownVector) < MinSlideAngle) {
		if (FVector::DotProduct(GetActorForwardVector(), SlideDir) < MinElecAngle) { //Add decreased Slide Force if Sliding in opposite direction of velocity
			
			//Try making this scale inversely based on speed
			//So the slower you're going, the greater the effect
			GetCharacterMovement()->AddForce(SlideDir * ElecSlideForce * ReducedSlideFactor);

			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Yellow, "Applying REDUCED Slide Physics!");
		}
		else { //Otherwise use normal slideforce
			GetCharacterMovement()->AddForce(SlideDir * ElecSlideForce);
			GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Green, "Applying NORMAL Slide Physics!");
		}



	}
	else { 
		GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Red, "Applying NO Slide Physics!");

		//GetCharacterMovement()->AddImpulse(SlideDir * ElecSlideForce);
	}



}

//While in Ball Mode, you shouldn't be able to accelerate very much, but you should still be able to turn tightly
//While in Ball Mode, Interpolate velocity to hold direction?
void APlayerCharacter::ElecBallMode() {


	//-----I advise AGAINST using a sphere trace, it causes some occasionally fucky collisions/rotations-----
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		(GetActorLocation() - (GetActorUpVector() * CentripetalDistanceOffset)),
		(GetActorLocation() - (GetActorUpVector() * CentripetalDistanceOffset)) - (GetActorUpVector() * CentripetalDistanceCheck),
		ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::ForDuration,
		BallModeHit, true, FLinearColor::Red, FLinearColor::Red, 1.0f);

	
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (BallModeHit.ImpactNormal * 1000.0f), FColor::Black, false, 10.1f, 0, 20.0f);
	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() - (GetActorUpVector() * CentripetalDistanceCheck), FColor::Blue, false, 1.1f, 0, 10.0f);


	
	//Debug if statement, should remove later


	if (ApplyElecGravity) {
		GetCharacterMovement()->AddForce(FVector::DownVector * ElecGravity);	
	}


	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (GetCharacterMovement()->Velocity), FColor::Green, false, 10.1f, 0, 20.0f);
	if (BallModeHit.bBlockingHit) {
		bool OnFlatSurface = FVector::DotProduct(BallModeHit.ImpactNormal, FVector::UpVector) > 0.98f;
		if (AlreadyInAir) {
			AlreadyInAir = false;

			//Convert current velocity into new forward velocity only if on curved surface
			if (!OnFlatSurface) {
				GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity.Size() * CalculateElecFloorInfluence(BallModeHit.ImpactNormal).GetSafeNormal();
			}
			else {
				//End if we're about to land on a flat surface
				if (FVector::DotProduct(GetCharacterMovement()->Velocity.GetSafeNormal(), FVector::DownVector) > 0.95f) {
					GEngine->AddOnScreenDebugMessage(-1, 5.1f, FColor::White, "LANDED WHILE GOING STRAIGHT DOWN!");
					ElecBallModeEnd();
					return;
				}
				GetCharacterMovement()->Velocity = FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0);
				
			}
		}

		//Only cancel out if we're less than min velocity, on the ground, and on a flat surface
		if (GetCharacterMovement()->Velocity.Size() < MinElecVelocity && OnFlatSurface) { //Exit Ball Mode
			ElecBallModeEnd();
			return;

		}

		//Only apply slide physics in not on a flat surface
		if (!OnFlatSurface) {
			ElecSlidePhysics();
		}

		if (ApplyElecDrag) {
			GetCharacterMovement()->AddForce(-GetActorForwardVector() * GetCharacterMovement()->Velocity.SizeSquared() * ElecGroundDragForce);

		}

		if (ApplyElecCentripetal) {
			FVector CentripetalForce = -BallModeHit.ImpactNormal * GetCharacterMovement()->Velocity.Size() * GetCharacterMovement()->Velocity.Size() * CentripetalForceScale;
			GetCharacterMovement()->AddForce(CentripetalForce);
			//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (CentripetalForce.GetSafeNormal() * 100.0f), FColor::White, false, 1.1f, 0, 20.0f);
		}
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (Cross * 100.0f), FColor::White, false, 1.1f, 0, 20.0f);


		GetCharacterMovement()->AddForce(GetActorForwardVector() * ElecForwardForce);


		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::White, "Normal Rotation!");
		SetActorRotation(
			UKismetMathLibrary::MakeRotationFromAxes(
				GetCharacterMovement()->Velocity.GetSafeNormal(),
				FVector::CrossProduct(BallModeHit.ImpactNormal, GetCharacterMovement()->Velocity.GetSafeNormal()),
				BallModeHit.ImpactNormal));

	

	
	}
	else { 
		AlreadyInAir = true;

		if (ApplyElecDrag) {

			//Prevent drag from actively reversing your direction in midair
			if (FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0).Size() > MinDragSpeed) {
				GetCharacterMovement()->AddForce(-GetActorForwardVector() * GetCharacterMovement()->Velocity.SizeSquared() * ElecAirDragForce);
			}

		}

		//Edge case if we fall down from top of loop
		if (FVector::DotProduct(FVector::DownVector, GetActorUpVector()) > 0.2f) {
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, "NORMALIZING ROTATION!");
			SetActorRotation(FRotator(0, GetActorRotation().Yaw, 0));
			return;
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Currently in the Air!");
			SetActorRotation(
				UKismetMathLibrary::MakeRotationFromAxes(
					FVector(GetCharacterMovement()->Velocity.X, GetCharacterMovement()->Velocity.Y, 0),
					GetActorRightVector(),
					FVector::UpVector));
		}



	}

}

void APlayerCharacter::ElecBallModeEnd() {

	//Set movement mode to ground if floor trace hits something, else go in air

	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), GetActorLocation(), GetActorLocation() - (GetActorUpVector() * CentripetalDistanceCheck), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, BallModeHit, true, FLinearColor::Red, FLinearColor::Red, 10.0f);
	if (BallModeHit.bBlockingHit) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		CurrentState = Normal;
	}
	else {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		CurrentState = InAir;
	}


	//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetVelocity(), FColor::Red, false, 10.1f, 0, 20.0f);
	ResetPhysics();
	SetActorRotation(FRotator(0, GetActorRotation().Yaw, GetActorRotation().Roll));
	CanElectric = true;
	InBallMode = false;
	GetCharacterMovement()->SetWalkableFloorAngle(60);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "Exited Ball Mode!");
}

void APlayerCharacter::ElecBallEnter() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Emerald, "Entering Ball Mode!");
	
	if (CurrentState == Ice) {
		IceEnd();
	}

	ElecForwardVelocity = GetCharacterMovement()->Velocity;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);


	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0;

	GetCharacterMovement()->AirControl = ElecAirControl;
	GetCharacterMovement()->FallingLateralFriction = ElecFallingLateralFriction;
	GetCharacterMovement()->GroundFriction = ElecFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = 0;
	InBallMode = true;

}

void APlayerCharacter::ElecDash() {
	StopJump();
	GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
	if (GetCharacterMovement()->IsFalling()) { //Jolt downwards
		LaunchCharacter(-GetActorUpVector() * ElectricDescentPower, true, true);
		GetCharacterMovement()->SetWalkableFloorAngle(90);
	}
	else if (!GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Size() > MinElecVelocity) { //Enter Ball Mode
		ElecBallEnter();
	}
	else { //Jolt Upwards
		GetCharacterMovement()->GravityScale = 0;
		GetCharacterMovement()->BrakingDecelerationFalling = 0;
		LaunchCharacter(GetActorUpVector() * ElectricAscentPower, true, true);
		BeginCountDown("Elec");
	}
	CurrentState = Electric;
	CanElectric = false;
}

//turn off collision when doing the grapple dash, but do a trace for objects to prevent going through walls?
//or just make the collider really small?
//or just teleport opposite side of the enemy?
void APlayerCharacter::ElecGrapple() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Elec Grapple!");
	GetWorldTimerManager().ClearTimer(TimeManager.GrappleHandle);
	//FVector Dir = CurrentLookTarget->GetActorLocation() - GetActorLocation();
	//Dir = FVector(Dir.X, Dir.Y, Dir.Z).GetSafeNormal();

	LaunchCharacter((CurrentLookTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal() * ElecGrapplePower, true, true);

	
	//SetActorEnableCollision(false);

	GetCapsuleComponent()->SetCollisionProfileName("Elec");

	GetCharacterMovement()->GroundFriction = 0;
	GetCharacterMovement()->BrakingDecelerationWalking = 0;
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->BrakingDecelerationFalling = 0;
	GetWorldTimerManager().SetTimer(TimeManager.ElecHandle, this, &APlayerCharacter::ElecGrappleEnd, ElecGrappleDuration);

}

void APlayerCharacter::ElecGrappleEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.ElecHandle);
	GetCapsuleComponent()->SetCollisionProfileName("Player");
	//SetActorEnableCollision(true);
	ResetPhysics();
	GetCharacterMovement()->StopMovementImmediately();
	if (GetCharacterMovement()->IsFalling()) {
		CurrentState = InAir;
	}
	else {
		CurrentState = Normal;
	}
}


void APlayerCharacter::ElecEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.ElecHandle);
	ResetPhysics();
	CurrentState = InAir;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Elec ended in air!");

	if (QueuedAction != Normal) {
		ActivateQueuedAction();
	}
	GetCharacterMovement()->SetWalkableFloorAngle(60);
}

void APlayerCharacter::ElecEndGrounded(const FHitResult& Hit) {
	GetWorldTimerManager().ClearTimer(TimeManager.ElecHandle);


	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Elec ended on ground!");
	ElecCarryOverPower = GetCharacterMovement()->Velocity.Size();
	if (QueuedAction == Fire) {
		FireBounce();
	}
	else if (QueuedAction == Ice) {
		IceDash();
	}
	else if (FVector::DotProduct(Hit.ImpactNormal, GetActorUpVector()) < 0.95f) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Current Velocity: %f"), GetCharacterMovement()->Velocity.Size()));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT("Dot Product: %f"), FVector::DotProduct(Hit.ImpactNormal, GetActorUpVector())));
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetVelocity() * (500.0f), FColor::Magenta, false, 10.1f, 0, 20.0f);

		ElecBallEnter();
		return;
	}
	else {
		GetWorldTimerManager().SetTimer(TimeManager.ElecHandle, this, &APlayerCharacter::ElecRecoilTimer, ElecRecoilTime);
		ElecRecoilActive = true;

	}
	
	GetCharacterMovement()->SetWalkableFloorAngle(60);
}

void APlayerCharacter::ElecRecoilTimer() {
	if (CurrentState != Ice && CurrentState != Fire) {
		ResetPhysics();
		CurrentState = Normal;
	}

	ElecRecoilActive = false;
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Elec recoil Ended!");
}

#pragma endregion

void APlayerCharacter::QueueAction(FState Action) {
	QueuedAction = Action;
	GetWorldTimerManager().SetTimer(TimeManager.ActionHandle, this, &APlayerCharacter::ActionQueueTimer, QueueBufferTime);

}

void APlayerCharacter::ActionQueueTimer() {
	GetWorldTimerManager().ClearTimer(TimeManager.ActionHandle);
	QueuedAction = Normal;
}


void APlayerCharacter::ActivateQueuedAction() {

	if (QueuedAction == Ice) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, "Activating Queued Ice!");
		IceDash();
	}
	else if (QueuedAction == Fire) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, "Activating Queued Fire!");
		FireDash();
	}
	else if (QueuedAction == Electric) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, "Activating Queued Electric!");
		ElecDash();
	}

	QueuedAction = Normal;
}


bool APlayerCharacter::SetCanGrappleEnemy(AActor* target, bool IsEnemy) {

	if (CurrentState == GrapplingEnemy) {
		return false;
	}

	if ((target->GetActorLocation() - GetActorLocation()).Size() > MinGrappleDistance) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Not close Enough!");

		return false;
	}

	FHitResult OutHit;

	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), GetActorLocation(), target->GetActorLocation(), ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, OutHit, true, FLinearColor::Red, FLinearColor::Green, 10.0f);

	if (OutHit.bBlockingHit) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Something's in the way!");
		return false;
	}

	float NewLookAtDot = FVector::DotProduct((target->GetActorLocation() - GetActorLocation()).GetSafeNormal(), GetActorForwardVector());

	if (NewLookAtDot < MinGrappleLookRange) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, "Not looking at right angle!");
		return false;
	}

	if (CurrentLookTarget != nullptr) {
		float CurrentLookAtDot = FVector::DotProduct((CurrentLookTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal(), GetActorForwardVector());

		if (NewLookAtDot > CurrentLookAtDot) {
			CurrentLookTarget = target;
			IsLookTargetEnemy = IsEnemy;
			//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, "Overriding target!");
			return true;
		}
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Failed to override target!");
		return false;
	}
	CurrentLookTarget = target;
	IsLookTargetEnemy = IsEnemy;
	return true;

}

void APlayerCharacter::CheckStillInGrappleRange() {

	if ((CurrentLookTarget->GetActorLocation() - GetActorLocation()).Size() > MinGrappleDistance) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, "Not close Enough!");
		CurrentLookTarget = nullptr;
		return;
	}


	float NewLookAtDot = FVector::DotProduct((CurrentLookTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal(), GetActorForwardVector());

	if (NewLookAtDot < MinGrappleLookRange) {
		//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Orange, "Not looking at right angle!");
		CurrentLookTarget = nullptr;
		return;
	}

}

void APlayerCharacter::GrappleEnemy() {
	if (CurrentLookTarget != nullptr) {
		if (CurrentState == Normal) {
			StopJump();
			GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
			CurrentState = GrapplingEnemy;
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Emerald, "Grappling Enemy!");
		}
		else if (CurrentState == InAir) {
			StopJump();
			GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
			CurrentState = GrapplingEnemy;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->GravityScale = 0;
			GetWorldTimerManager().SetTimer(TimeManager.GrappleHandle, this, &APlayerCharacter::GrappleEnemyEnd, GrappleDuration);
		}


	}



}

void APlayerCharacter::GrappleEnemyEnd() {
	GetWorldTimerManager().ClearTimer(TimeManager.GrappleHandle);
	GetCharacterMovement()->GravityScale = BaseGravity;
	CurrentState = InAir;
}

void APlayerCharacter::ReleaseGrappleEnemy() {
	//CurrentTarget = nullptr;
	CurrentLookTarget = nullptr;
}

void APlayerCharacter::ChargeDive() {

}


void APlayerCharacter::BeginGrapple() {

}


void APlayerCharacter::EndGrapple() {

}



void APlayerCharacter::ResetPhysics() {
	GetCharacterMovement()->AirControl = BaseAirControl;
	GetCharacterMovement()->RotationRate = BaseRotationRate;
	GetCharacterMovement()->FallingLateralFriction = BaseLateralFriction;
	GetCharacterMovement()->GravityScale = BaseGravity;
	GetCharacterMovement()->GroundFriction = BaseFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = BaseBrakingDecelerationWalking;
	GetCharacterMovement()->BrakingDecelerationFalling = BaseBrakingDecelerationFalling;
	GetCharacterMovement()->BrakingFrictionFactor = BaseBrakingFrictionFactor;
}

void APlayerCharacter::ResetJumpValues() {
	CanBuffer = false;
	//CurrentState = Normal;
	CanDoubleJump = true;
	WallNormal = { 0,0,0 };
	GetCharacterMovement()->JumpZVelocity = baseJumpVelocity;
	CanDash = true;
	JumpCurrentCount = 0;
	DoubleJumped = false;
}





void APlayerCharacter::UpdateFace(FFaceAnimationState state, bool HealthType) {
	if (HealthType) {
		if (HP <= 5 && HP > 0) {
			UpdateFace(Hurt);
			return;
		}
		else if (HP == MaxHP) {
			UpdateFace(Smile);
			return;
		}
		else {
			UpdateFace(Frown);
			return;
		}
	}
	FaceState = state;

	switch (FaceState) {
	case Frown:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Frowning");
		Eyes->SetTextureParameterValue("State", EyeTextures[0]);
		Mouth->SetTextureParameterValue("State", MouthTextures[0]);
		break;

	case Smile:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Smiling!");
		Eyes->SetTextureParameterValue("State", EyeTextures[0]);
		Mouth->SetTextureParameterValue("State", MouthTextures[1]);
		break;

	case Happy:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Happy");
		Eyes->SetTextureParameterValue("State", EyeTextures[0]);
		Mouth->SetTextureParameterValue("State", MouthTextures[2]);
		break;

	case Awe:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Awe");
		Eyes->SetTextureParameterValue("State", EyeTextures[2]);
		Mouth->SetTextureParameterValue("State", MouthTextures[3]);
		break;

	case Worried:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Worried");
		Eyes->SetTextureParameterValue("State", EyeTextures[1]);
		Mouth->SetTextureParameterValue("State", MouthTextures[4]);
		break;

	case Hurt:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Hurt!");
		Eyes->SetTextureParameterValue("State", EyeTextures[3]);
		Mouth->SetTextureParameterValue("State", MouthTextures[5]);
		break;

	case Sad:
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Sad!");
		Eyes->SetTextureParameterValue("State", EyeTextures[0]);
		Mouth->SetTextureParameterValue("State", MouthTextures[5]);
		break;


	}
}

void APlayerCharacter::IncrementJumpCount() {
	JumpCurrentCount++;
}


void APlayerCharacter::GrappleFocus(FVector Location) {

}

void APlayerCharacter::BeginCountDown(FName Type) {
	if (Type == "Attack") {
		TimeManager.AttackTimer = AttackCountdownTime;
		GetWorldTimerManager().SetTimer(TimeManager.AnimationHandle, this, &APlayerCharacter::AdvanceAttackTimer, 0.1f, true);
	}
	else if (Type == "Invincibility") {
		TimeManager.InvincibilityTimer = InvincibilityCountdownTime;
		GetWorldTimerManager().SetTimer(TimeManager.InvincibilityHandle, this, &APlayerCharacter::AdvanceInvincibilityTimer, 1.0f, true);
	}
	else if (Type == "Dash") {
		TimeManager.DashTimer = DashCountdownTime;
		GetWorldTimerManager().SetTimer(TimeManager.DashHandle, this, &APlayerCharacter::AdvanceDashTimer, 0.1f, true);
	}
	else if (Type == "Heal") {
		TimeManager.HealTimer = HealCountdownTime;
		GetWorldTimerManager().SetTimer(TimeManager.HealHandle, this, &APlayerCharacter::AdvanceHealTimer, 1.0f, true, 1.0f);
	}
	else if (Type == "Coyote") {
		GetWorldTimerManager().ClearTimer(TimeManager.CoyoteHandle);
		CoyoteBuffer = true;
		GetWorldTimerManager().SetTimer(TimeManager.CoyoteHandle, this, &APlayerCharacter::CoyoteTimer, CoyoteTime);


		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Entering Coyote Time!");
	}
	else if (Type == "Ice") {
		GetWorldTimerManager().ClearTimer(TimeManager.IceHandle);
		GetWorldTimerManager().SetTimer(TimeManager.IceHandle, this, &APlayerCharacter::IceEnd, IceDuration);
	}
	else if (Type == "Fire") {
		GetWorldTimerManager().ClearTimer(TimeManager.FireHandle);
		GetWorldTimerManager().SetTimer(TimeManager.FireHandle, this, &APlayerCharacter::FireEnd, FireDuration);
	}
	else if (Type == "Elec") {
		GetWorldTimerManager().ClearTimer(TimeManager.ElecHandle);
		GetWorldTimerManager().SetTimer(TimeManager.ElecHandle, this, &APlayerCharacter::ElecEnd, ElecDuration);
	}


}

void APlayerCharacter::CoyoteTimer() {
	CoyoteBuffer = false;
	GetWorldTimerManager().ClearTimer(TimeManager.CoyoteHandle);
	if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling()) { //If you didn't jump while coyote time is active, increment jump anyways
		JumpCurrentCount++;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Coyote Time over!");
}


#pragma region Overrides


//All we add to this is a check to prevent jump count from incrementing if in coyote time
void APlayerCharacter::CheckJumpInput(float DeltaTime) {
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (GetCharacterMovement())
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			const bool bFirstJump = JumpCurrentCount == 0;
			if (bFirstJump && GetCharacterMovement()->IsFalling() && !CoyoteBuffer) 
			{
				JumpCurrentCount++;
			}

			const bool bDidJump = CanJump() && GetCharacterMovement()->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}


//Activate coyote time, optional apex notification
void APlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) {
	if (!bPressedJump || !GetCharacterMovement()->IsFalling())
	{
		if (NotifyJumpApexOnMovementChanged) {
			GetCharacterMovement()->bNotifyApex = false;
		}
		ResetJumpState();


	}

	if (InBallMode) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, "CHANGED STATE WHILE IN BALL MODE");
	}
	
	if (!GetCharacterMovement()->IsFalling() && InBallMode) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, "LANDED WHILE IN BALL MODE");
	}


	if (GetCharacterMovement()->IsFalling() && InBallMode) {
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, "ENTERED FALLING STATE WHILE IN BALL MODE");
	}

	// Record jump force start time for proxies. Allows us to expire the jump even if not continually ticking down a timer.
	if (bProxyIsJumpForceApplied && GetCharacterMovement()->IsFalling())
	{
		ProxyJumpForceStartedTime = GetWorld()->GetTimeSeconds();
	}

	K2_OnMovementModeChanged(PrevMovementMode, GetCharacterMovement()->MovementMode, PrevCustomMode, GetCharacterMovement()->CustomMovementMode);
	MovementModeChangedDelegate.Broadcast(this, PrevMovementMode, PrevCustomMode);


	if (GetCharacterMovement()->IsFalling() && !bPressedJump) { //Activate Coyote Time when walking off ledge
		if (CurrentState == Slide) {
			CurrentState = Dive;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, "returning to dive state after falling while sliding");
		}
		else if (CurrentState != Dive && CurrentState != Slide && CurrentState != Bonk && CurrentState != Ice && CurrentState != Fire && CurrentState != Electric) {
			BeginCountDown(TimeManager.Coyote);

			
		}

	}


}



//There's a check for flying and the option to reset gravity back to normal when clearing the input.
void APlayerCharacter::ClearJumpInput(float DeltaTime) {
	if (CurrentState != Flying) {
		if (bPressedJump)
		{
			JumpKeyHoldTime += DeltaTime;

			// Don't disable bPressedJump right away if it's still held.
			// Don't modify JumpForceTimeRemaining because a frame of update may be remaining.
			if (JumpKeyHoldTime >= GetJumpMaxHoldTime())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Clearing Jump Input!!!");
				bPressedJump = false;

				if (ResetGravityOnClearJumpInput) {
					GetCharacterMovement()->GravityScale = BaseGravity;
				}


			}
		}
		else
		{
			JumpForceTimeRemaining = 0.0f;
			bWasJumping = false;
		}
	}
}




//Just an optional check for resetting gravity on jump apex
void APlayerCharacter::NotifyJumpApex() {
	// Call delegate callback
	if (CurrentState != Flying) {
		if (ResetGravityOnApex) {
			GetCharacterMovement()->GravityScale = BaseGravity; //Reset back to base value on Peak Jump
		}

		if (OnReachedJumpApex.IsBound())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "At Apex!!");
			OnReachedJumpApex.Broadcast();

		}
	}

}


//reset some jump related stuff on landing
void APlayerCharacter::Landed(const FHitResult& Hit) {

	Super::Landed(Hit);

	if (InBallMode) {

		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, "ENTERING LANDED IN BALL MODE");
		return;
	}

	CanBuffer = false;
	if (CurrentState == Flying) {
		ExitFlight();
	}
	if (CurrentState == Dive) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Sliding!");
		CurrentState = Slide;
	}
	else if (CurrentState == Fire) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Landed in Fire State!");
		CanIce = true;
		FireEndGrounded();

	}
	else if (CurrentState == Electric) {
		if (InBallMode) {
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, "ALREADY IN BALL MODE");

		}
		CanIce = true;
		ElecEndGrounded(Hit);
		
	}
	else if (CurrentState != Bonk && CurrentState != Dive && CurrentState != Slide && CurrentState != Ice) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Landed");
		CurrentState = Normal;
		CanIce = true;
	}

	if (IceGrappling) {
		IceGrappleEnd(false);
	}
	CanFire = true;
	CanElectric = true;
	
	ResetJumpValues();


	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Landed");
}


//Nothing was really changed, reset some values on wall cling
//might need to re-add inout buffering here
void APlayerCharacter::ResetJumpState() {
	bPressedJump = false;
	bWasJumping = false;
	JumpKeyHoldTime = 0.0f;
	JumpForceTimeRemaining = 0.0f;

	if (GetCharacterMovement() && !GetCharacterMovement()->IsFalling())
	{
		JumpCurrentCount = 0;
		JumpCurrentCountPreJump = 0;

		if (CurrentState == WallCling) { 
			//ResetJumpValues();
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Reset Jump State!");
}

#pragma endregion

#pragma region defunctShit

void APlayerCharacter::AdvanceAttackTimer() {

}



void APlayerCharacter::AdvanceInvincibilityTimer() {

}



void APlayerCharacter::AdvanceDashTimer() {

}



void APlayerCharacter::AdvanceBoostTimer() {

}



void APlayerCharacter::AdvanceHealTimer() {

}

void APlayerCharacter::ResetAttackState() {

}


void APlayerCharacter::LoseHealth(float amount) {

}


void APlayerCharacter::LoseMana(float amount) {

}


void APlayerCharacter::RestoreHealth(float amount) {

}


void APlayerCharacter::RestoreMana(float amount) {

}


void APlayerCharacter::StartHealing() {

}


void APlayerCharacter::StopHealing() {

}


void APlayerCharacter::LandToJump() {

}





void APlayerCharacter::AdvanceFlightAccelTimer() {

}

void APlayerCharacter::ExitFlight() {

}

#pragma endregion
