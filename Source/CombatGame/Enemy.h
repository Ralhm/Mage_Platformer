// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionTypes.h"
#include "Enemy.generated.h"


UCLASS()
class COMBATGAME_API AEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AActor* TargetPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) //Base Value
		int32 LineOfSightTimer;

	int32 ChaseTimer; //Stop Chasing player if line of sight is broken for long enough

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	float FlinchChance;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int AttackSelection;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int IdleSelection;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> IdleMontages;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
	TArray<UAnimMontage*> AttackMontages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AActor> Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ChasingPlayer;

	UPROPERTY(BlueprintReadWrite)
	bool CanSeePlayer;

	UPROPERTY(BlueprintReadWrite)
	bool Dead;

	UPROPERTY(EditAnywhere)
	bool WaveEnemy;

	UPROPERTY(BlueprintReadWrite)
	bool Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanMove;

	bool CanAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IdleChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackChance;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StrafeLocation;



	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Idling;

	FTimerHandle ChaseHandle;
	FTimerHandle MontageHandle;

	UPROPERTY(EditAnywhere)
	USoundBase* DetectSound;

	virtual void Death();

	UFUNCTION()
	virtual void DeathAction();


	UFUNCTION(BlueprintCallable)
	void Detect(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION(BlueprintCallable)
	void Pursue();

	UFUNCTION(BlueprintCallable)
	void DoneIdling();

	UFUNCTION(BlueprintCallable)
	void SelectIdle();

	UFUNCTION(BlueprintCallable)
	virtual void BeginAttack();

	UFUNCTION(BlueprintCallable)
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable)
	virtual void ShootProjectile();

	UFUNCTION()
	void ChaseEnd();

	UFUNCTION()
	void CancelIdle();

	FGenericTeamId TeamId;
};
