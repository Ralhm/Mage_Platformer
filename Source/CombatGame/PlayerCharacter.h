// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum FAttackType {
	Nothing UMETA(DisplayName = "Nothing"),
	Light UMETA(DisplayName = "Light"),
	Heavy UMETA(DisplayName = "Heavy"),
	Melee UMETA(DisplayName = "Melee"),
	Magic UMETA(DisplayName = "Magic")
};

UENUM(BlueprintType)
enum FMoveDirection {
	Right UMETA(DisplayName = "Right"),
	Back UMETA(DisplayName = "Back"),
	Left UMETA(DisplayName = "Left"),
	Fore UMETA(DisplayName = "Fore")
};

UENUM(BlueprintType)
enum FState {
	Normal UMETA(DisplayName = "Normal"),
	Healing UMETA(DisplayName = "Healing"),
	InAir UMETA(DisplayName = "InAir"),
	Attacking UMETA(DisplayName = "Attacking"),
	InDodge UMETA(DisplayName = "InDodge"),
	Dead UMETA(DisplayName = "Dead"),
	Sprinting UMETA(DisplayName = "Sprinting"),
	WallSliding UMETA(DisplayName = "WallSlide"),
	WallJump UMETA(DisplayName = "WallJump"),
	Swinging UMETA(DisplayName = "Swinging"),
	WallCling UMETA(DisplayName = "WallCling"),
	WallClimb UMETA(DisplayName = "WallClimb"),
	Dive UMETA(DisplayName = "Dive"),
	TurnAround UMETA(DisplayName = "TurnAround"),
	Damaged UMETA(DisplayName = "Damaged"),
	Slide UMETA(DisplayName = "Slide"),
	Bonk UMETA(DisplayName = "Bonk"),
	WallRun UMETA(DisplayName = "WallRun"),
	ChargingBoost UMETA(DisplayName = "ChargingBoost"),
	Flying UMETA(DisplayName = "Flying"),
	Ice UMETA(DisplayName = "Ice"),
	Fire UMETA(DisplayName = "Fire"),
	Electric UMETA(DisplayName = "Electric"),
	GrapplingEnemy UMETA(DisplayName = "GrapplingEnemy")

};

UENUM(BlueprintType)
enum FFaceAnimationState {
	Smile UMETA(DisplayName = "Smile"), //Full Health State
	Frown UMETA(DisplayName = "Frown"), //Not full health
	Happy UMETA(DisplayName = "Happy"),
	Awe UMETA(DisplayName = "Awe"),
	Hurt UMETA(DisplayName = "Hurt"), //Low Health
	Sad UMETA(DisplayName = "Sad"),
	Blinking UMETA(DisplayName = "Blinking"),
	Worried UMETA(DisplayName = "Worried")

};

USTRUCT(BlueprintType)
struct FCountDownType {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Attack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Invincibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Dash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Heal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Coyote;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Boost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName FlightAccel;

	float AttackTimer;

	float InvincibilityTimer;

	float DashTimer;

	float HealTimer;

	float CoyoteTimer;

	float BoostTimer;

	float FlightAccelTimer;

	float IceGrappleTimer;

	FTimerHandle AnimationHandle; //For resetting the animation montage if player stops attacking

	FTimerHandle ActionHandle; //For resetting the animation montage if player stops attacking

	FTimerHandle InvincibilityHandle; //For resetting the animation montage if player stops attacking

	FTimerHandle DashHandle; //For Cooldown on Dash move

	FTimerHandle HealHandle; //For Cooldown on Heal move

	FTimerHandle CoyoteHandle; //For Cooldown on Dash move

	FTimerHandle BoostHandle; //For Charging your dash

	FTimerHandle GrappleHandle; //For Breaking out of grapple after a bit

	FTimerHandle SkidHandle; //Skid until speed == 0;

	FTimerHandle IceHandle; 
	FTimerHandle FireHandle; 
	FTimerHandle ElecHandle; 

	FTimerHandle FlightAccelHandle; //For Charging your dash

	FCountDownType() {
		Attack = FName(TEXT("Attack"));
		Action = FName(TEXT("Action"));
		Invincibility = FName(TEXT("Invincibility"));
		Dash = FName(TEXT("Dash"));
		Heal = FName(TEXT("Heal"));
		Coyote = FName(TEXT("Coyote"));
		Boost = FName(TEXT("Boost"));
		FlightAccel = FName(TEXT("FlightAccel"));
	}

};



UCLASS(BlueprintType)
class COMBATGAME_API APlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	virtual void CheckJumpInput(float DeltaTime) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void ClearJumpInput(float DeltaTime) override;

	virtual void NotifyJumpApex() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void ResetJumpState() override;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* Outline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Staff;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* StaffOutline;


	//target that can be grappled
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* CurrentLookTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector GrappleTargetPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsLookTargetEnemy;

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void LookUpAtRate(float Rate);

	UFUNCTION()
	void TurnAtRate(float Rate);

	UFUNCTION()
	void StartJump();

	UFUNCTION(BlueprintCallable)
	void StopJump();

	UFUNCTION()
	void StartSprint();

	UFUNCTION()
	void StopSprint();

	UFUNCTION()
	void DiveEnter();


	UFUNCTION(BlueprintCallable)
	void DiveExit();

	UFUNCTION()
	void ChargeDive();

	UFUNCTION(BlueprintCallable)
	void BeginGrapple();

	UFUNCTION(BlueprintCallable)
	void EndGrapple();

	UFUNCTION(BlueprintCallable)
	void TugAction();


	UFUNCTION(BlueprintCallable)
	void TugActionEnd();


	UFUNCTION(BlueprintCallable)
	void ResetPhysics();


	UFUNCTION(BlueprintCallable)
	void ResetJumpValues();

	UFUNCTION(BlueprintCallable)
	void ResetAttackState();

	UFUNCTION(BlueprintCallable)
	void ExitFlight();




	UFUNCTION(BlueprintCallable)
	void LandToJump();

	UFUNCTION(BlueprintCallable)
	void IceSlidePhysics();

	UFUNCTION(BlueprintCallable)
	void ElecSlidePhysics();

	UFUNCTION(BlueprintCallable)
	const FVector CalculateFloorInfluence(const FVector& FloorNormal);

	UFUNCTION(BlueprintCallable)
	const FVector CalculateElecFloorInfluence(const FVector& FloorNormal);


	UFUNCTION(BlueprintCallable)
	void UpdateFace(FFaceAnimationState state, bool HealthType = false);

	UFUNCTION(BlueprintCallable) //Blueprintcallable so we can increase jump count value when cancelling a slide in blueprints
	void IncrementJumpCount();

	UFUNCTION(BlueprintCallable)
	void GrappleFocus(FVector Location);

	UFUNCTION(BlueprintCallable)
	void TurnAroundCheck();


	UFUNCTION(BlueprintCallable)
	void IceAction();


	UFUNCTION(BlueprintCallable)
	void IceDash();

	UFUNCTION(BlueprintCallable)
	void IceGrappleBegin();


	UFUNCTION(BlueprintCallable)
	void IceGrappleForceHorizontal();

	UFUNCTION(BlueprintCallable)
	void IceGrappleForceVertical();

	UFUNCTION(BlueprintCallable)
	void IceGrappleEnd(bool Launch = true);

	UFUNCTION(BlueprintCallable)
	void IceGrappleTimerHorizontal();

	UFUNCTION(BlueprintCallable)
	void IceGrappleTimerVertical();

	UFUNCTION(BlueprintCallable)
	void FireAction();

	UFUNCTION(BlueprintCallable)
	void FireDash();

	UFUNCTION(BlueprintCallable)
	void FireDashEnemy();

	UFUNCTION(BlueprintCallable)
	void FireBounce(float Power = 0);

	UFUNCTION(BlueprintCallable)
	void FireBounceEnemy();

	UFUNCTION(BlueprintCallable)
	void FireGrapple();

	UFUNCTION(BlueprintCallable)
	void ElecAction();

	UFUNCTION(BlueprintCallable)
	void ElecDash();

	UFUNCTION(BlueprintCallable)
	void ElecBallEnter();

	UFUNCTION(BlueprintCallable)
	void ElecBallMode();

	UFUNCTION(BlueprintCallable)
	void ElecBallModeEnd();

	UFUNCTION(BlueprintCallable)
	void ElecGrapple();

	UFUNCTION(BlueprintCallable)
	void ElecGrappleEnd();

	UFUNCTION(BlueprintCallable)
	bool SetCanGrappleEnemy(AActor* target, bool IsEnemy);

	UFUNCTION(BlueprintCallable)
	void CheckStillInGrappleRange();

	UFUNCTION(BlueprintCallable)
	void GrappleEnemy();

	UFUNCTION(BlueprintCallable)
	void GrappleEnemyEnd();

	UFUNCTION(BlueprintCallable)
	void ReleaseGrappleEnemy();

	UFUNCTION(BlueprintCallable)
	void GrappleEnd();



	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte < FState > CurrentState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte < FState > QueuedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray <TEnumAsByte <EObjectTypeQuery> > ObjectTypes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray< AActor* > ActorsToIgnore;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool ResetGravityOnApex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool NotifyJumpApexOnMovementChanged;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool ResetGravityOnClearJumpInput;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float BaseTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float BaseLookUpRate;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float DashCancelBoostAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float SlidecancelBoostAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float ClimbBoost;






	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties") //Vertical Component of Dash
		float DashUpPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float MaxBoostPower; //How much extra power can the player get when boosting a dive?

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float BoostIncrement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float DoubleJumpPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float WallSlideGravity = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float WallSlideSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float WallJumpPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float WallJumpAirControl;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float WallRunJumpPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float DodgePower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float SprintSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float WallRunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float SprintFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float SlipSpeedBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float TurnAroundFriction;

	//Would probably be better as an enemy variable but I don't care to change it right now
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties") //How far does one get sent when launched by damage?
		float LaunchDamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float jumpingGravity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float CoyoteTime; //How much time does a player have after walking off a ledge to still jump

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MovementProperties")
	float QueueBufferTime; //How much time does a player have after walking off a ledge to still jump


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	bool InBallMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties") //Horizontal Component of Dash
		float DivePower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties") 
		float DiveAirControl;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties")
		float DiveLateralFriction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties") 
		float DiveGravity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties") 
		float DiveFriction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties")
		float DiveBrakingDecelerationWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DiveProperties") 
		float DiveBrakingFriction;

	//How far Forward is the player boosted on initial Ice Boost
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceHorizontalPower = 750.0f;

	//How Up Forward is the player boosted on initial Ice Boost
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceVerticalPower = 400.0f;

	//How much control does the player have over turning/accelerating
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceMovementPower = 1300.0f;

	//How much is the character affected by sliding? (Used in Ice and Electric)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MovementProperties")
	float IceSlideForce = 4000000.0;

	//How high can the player jump when in Ice Mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceJumpPower = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceFrictionTime = 0.5f;

	//How high does the player boost when colliding with a wall mid-air in Ice Mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceBouncePower = 1600.0f;

	//How long does Ice Mode last
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceDuration = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties") 
	float IceAirControl = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties") 
	float IceLateralFriction = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties") 
	float IceGravity = 1.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceFriction = 0.0f;

	//Keep in mind that these values only come into play when not holding a button
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceBrakingDecelerationWalking = 600.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties")
	float IceBrakingDecelerationFalling = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IceProperties") 
	float IceBrakingFriction = 0.6;




	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
	float FireDashPower = 900.0;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireBouncePower = 350.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireBounceEnemyPower = 800.0;

	//What fraction of the current speed gets carried over into the boost?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireBounceBoost = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireBounceAirControl = 4.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
		float FireAirControl;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireDuration = 0.5;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireCarryOverFactor = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
		float FireDelayTime = 0.6;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
		float FireLateralFriction = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
		float FireGravity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
		float FireFriction = 0.1;

		//Keep in mind that these values only come into play when not holding a button
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
	float FireBrakingDecelerationFalling = 200.0;

	//Keep in mind that these values only come into play when not holding a button
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties")
	float FireBrakingDecelerationWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FireProperties") 
	float FireBrakingFrictionFactor;

	//Are we in recoild from landing from a jolt?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	bool ElecRecoilActive;

	//Power with which you ascened when jolting up
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElectricAscentPower = 1200.0;

	//How quickly do you descend when jolting down?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElectricDescentPower = 1000.0;

	//How long does the jolt up last?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecDuration = 0.05;

	//Gravity Experienced while in ball mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecGravity = 110000.0;

	//Minimum velocity needed to maintain Ball Mode
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float MinElecVelocity = 300.0f;

	//Duration at which you can briefly not move after landing on a flat surface
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecRecoilTime = 0.5;



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecFallingLateralFriction = 0.4;

	//If the player is going up, adjust amount of force experienced
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float MinElecAngle = 0.0f;

	//Prevent max sliding from occurring when going upside down during loop
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float MinSlideAngle = -0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float MinDragSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecFriction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecAirControl = 4.0f;

	//Keep in mind that these values only come into play when not holding a button
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float CentripetalForceScale = 0.1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float CentripetalDistanceCheck = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ForwardCheck = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float CentripetalDistanceOffset = 40.0;

	//How much is the character affected by sliding
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecSlideForce = 9000000.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecForwardForce = 600.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecGroundDragForce = 0.005;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecAirDragForce = 0.0007;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ElecSteeringPower = 325.0;

	//How much is the slide force modified in it's applied in the oppisite direction?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	float ReducedSlideFactor = 0.1;




	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	bool ApplyElecCentripetal = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	bool ApplyElecDrag = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	bool ApplyElecGravity = false;

	//Debug for testing differences between hard setting velocity via calculations and using builtin forces
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	bool UseForces = false;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ElectricProperties")
	FVector ElecForwardVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float MinGrappleDistance;

	//Dot product range
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float MinGrappleLookRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float GrappleDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float ElecGrapplePower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float ElecGrappleDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float FireGrapplePower;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float IceGrapplePower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float IceGrappleLaunchPower;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float IceGrappleDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float IceGrappleRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	bool IceGrappling;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	bool IceGrappleLaunch;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GrappleProperties")
	float IceGrappleDir;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float MagicPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float HP;

	UPROPERTY(EditAnywhere, Category = "CombatProperties")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float HealRate;

	UPROPERTY(VisibleAnywhere, Category = "CombatProperties")
	float Mana;

	UPROPERTY(EditAnywhere, Category = "CombatProperties")
	float MaxMana;

	//How much Mana you regain upon htting an enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float Soul;
	//How long are you invincible for after taking damage?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float InvincibilityTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float LightSpellCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatProperties")
	float HeavySpellCost;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues") //Let this be the gravity value for Falling
		float BaseGravity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseAirControl;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	FRotator BaseRotationRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseLateralFriction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseFriction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseBrakingDecelerationWalking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseBrakingDecelerationFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseBrakingFrictionFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float baseJumpVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseValues")
	float BaseWalkSpeed;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* TugMontage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float TugMontageSpeed;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation") //State Machine For animating the face materials
		TEnumAsByte < FFaceAnimationState > FaceState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UMaterialInstanceDynamic* Mouth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UMaterialInstanceDynamic* Eyes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<UTexture*> EyeTextures;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TArray<UTexture*> MouthTextures;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Animation")
	float SwingAnimDot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	class UNiagaraSystem* ManaParticles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particles")
	class UNiagaraComponent* ManaComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float HealthPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ManaPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float DashPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BaseValues")
	FVector OldDir;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanDoubleJump = true; //Shouldn't be able to double jump after sprint jump

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool JumpBuffer; //Draw a line down, if it touches the ground while player is still in air, then buffer the input until player lands

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanBuffer; //Is the player close enough to the ground to be able to buffer the jump input?

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) //Should not be able to Coyote after jumping, should only be able to coyote if you haven't jumped yet (Just check if JumpCount is 1 or 0)
		bool CoyoteBuffer; //Can the player still jump after falling off a ledge for a moment?

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool DoubleJumped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanSkid; //The player can still skid if you hold the opposite direction when landing, just set the value to false when in the air and true a few frames after landing

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BufferedJump; //Did the player do a buffered Jump (for smooth animation transtion from falling to jump_start, if true, transition, then set to false upon transition)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector CurrentHoldDir; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool Tugging;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool MovingRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool MovingForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float RightAxisVal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ForwardAxisVal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanIce; 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanIceBounce;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanElectric;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ThrottleUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ThrottleDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Invincible;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) //Wall running bools
		bool IsWallRightSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsWallLeftSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool OnRightSide;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool OnLeftSide;


	bool CanDash;

	bool WallJumped; //If you've jumped on a wall

	bool EndOfCombo;

	bool DashBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanGrapple;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BoostPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ElecCarryOverPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector WallNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector RespawnPoint; //For when the player hits a kill box, set players point there. Only Set when the player enters falling state from grounded state

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector RopeLaunchDirection; //In which direction does the player get launched when letting go of the rope?

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector DashDirection; //In Which Direction does the player launch when dashing?

	FCountDownType TimeManager;

	UPROPERTY(EditAnywhere)
	float AttackCountdownTime;

	UPROPERTY(EditAnywhere)
	float InvincibilityCountdownTime;

	UPROPERTY(EditAnywhere)
	float DashCountdownTime;

	UPROPERTY(EditAnywhere)
	float HealCountdownTime;

	UPROPERTY(EditAnywhere)
	float BoostCountdownTime; //How long does the player have until boost is Fully Charged? (Force the player to fire a second or two after reaching max Power)


	UFUNCTION(BlueprintCallable)
	void BeginCountDown(FName Type);

	UFUNCTION()
	void AdvanceAttackTimer();

	UFUNCTION()
	void AdvanceInvincibilityTimer();

	UFUNCTION()
	void AdvanceDashTimer();

	UFUNCTION()
	void AdvanceBoostTimer();

	UFUNCTION()
	void AdvanceHealTimer();

	UFUNCTION()
	void CoyoteTimer();

	UFUNCTION()
	void SkidTimer();

	UFUNCTION()
	void QueueAction(FState Action);

	UFUNCTION()
	void ActionQueueTimer();

	UFUNCTION()
	void FireEnd();

	UFUNCTION()
	void FireEndGrounded();


	UFUNCTION()
	void IceEnd();

	UFUNCTION()
	void IceFrictionTimer();

	UFUNCTION()
	void FireDelayTimer();


	UFUNCTION()
	void ElecEnd();


	UFUNCTION()
	void ElecEndGrounded(const FHitResult& Hit);

	UFUNCTION()
	void ElecRecoilTimer();

	UFUNCTION()
	void ActivateQueuedAction();



	UFUNCTION(BlueprintCallable)
	void IceBounce();


	UFUNCTION()
	void AdvanceFlightAccelTimer();

	FGenericTeamId TeamId;

	UFUNCTION()
	void LoseHealth(float amount);

	UFUNCTION()
	void LoseMana(float amount);

	UFUNCTION()
	void RestoreHealth(float amount);

	UFUNCTION()
	void RestoreMana(float amount);

	UFUNCTION()
	void StartHealing();

	UFUNCTION()
	void StopHealing();

	FHitResult BallModeHit;

	FHitResult BallModeForwardHit;

	bool AlreadyInAir;
};
