// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Components/CapsuleComponent.h"

#include "EnemyAIController.h"


#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TeamId = FGenericTeamId(0);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemy::Detect(AActor* Actor, FAIStimulus Stimulus) {
	
	if (Stimulus.WasSuccessfullySensed()) {
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if (!ChasingPlayer) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), DetectSound, GetActorLocation());
			ChasingPlayer = true;


			if (Idling) {
				CancelIdle();
			}



		}
		if (AIController) {
			AIController->Board->SetValueAsBool("Chasing", true);

		}
		CanSeePlayer = true;
		GetWorldTimerManager().ClearTimer(ChaseHandle);



	}
	else {
		ChaseTimer = LineOfSightTimer;
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, Actor->GetName());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, TEXT("Losing Sight of Player!"));
		GetWorldTimerManager().SetTimer(ChaseHandle, this, &AEnemy::ChaseEnd, LineOfSightTimer, false);
		CanSeePlayer = false;

	}
	
}

void AEnemy::Death() {

}


void AEnemy::DeathAction() {

}





void AEnemy::Pursue() {
	if (TargetPlayer != NULL) {

		if (CanMove) {
			AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
			if (AIController) {
				GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Moving To Player..."));
				AIController->MoveToActor(TargetPlayer, -1, false, true, false);
			}

		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("CAN'T MOVE!"));
		}

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Player Not Found!"));
	}

}


void AEnemy::DoneIdling() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("Done Idling!"));
	GetWorldTimerManager().ClearTimer(MontageHandle);
	AEnemyAIController* myAIController = (AEnemyAIController*)GetController();
	if (myAIController) {
		myAIController->Board->SetValueAsBool("IsIdling", false);
	}

	Idling = false;
}




void AEnemy::BeginAttack() {
	//UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSound, GetActorLocation(), 0.6f);

	if (CanAttack) {
		Attacking = true;
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		if (AIController) {
			AIController->StopMovement();
		}
		AttackSelection = FMath::RandRange(0, AttackMontages.Num() - 1);
		UAnimInstance* instance = GetMesh()->GetAnimInstance();
		if (instance) {
			float Montage_Length = instance->Montage_Play(AttackMontages[AttackSelection], 1.0f);
			GetWorldTimerManager().SetTimer(MontageHandle, this, &AEnemy::EndAttack, Montage_Length);
		}
		AIController->Board->SetValueAsBool("IsAttacking", true);
		GetCharacterMovement()->MaxWalkSpeed = 0;
		CanMove = false;

	}
}


void AEnemy::EndAttack() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Black, "Ending Enemy Attack Notify");
	Attacking = false;
	AttackSelection = 0;
	CanMove = true;

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	AIController->Board->SetValueAsBool("IsAttacking", false);
}


void AEnemy::ShootProjectile() {

}


void AEnemy::ChaseEnd() {
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, TEXT("No longer chasing!"));
	ChasingPlayer = false;



	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetWorldTimerManager().ClearTimer(ChaseHandle);

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController) {
		AIController->Board->SetValueAsBool("Chasing", false);
		AIController->Board->SetValueAsBool("IsIdling", true);
		AIController->StopMovement();
		AIController->SetFocus(nullptr);
		GetWorldTimerManager().SetTimer(MontageHandle, this, &AEnemy::DoneIdling, 4.0f);
	}

}

void AEnemy::SelectIdle() {
	UAnimInstance* instance = GetMesh()->GetAnimInstance();
	if (instance) {
		Idling = true;

		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		AIController->Board->SetValueAsBool("IsIdling", true);

		IdleSelection = FMath::RandRange(0, IdleMontages.Num() - 1);
		float Montage_Length = instance->Montage_Play(IdleMontages[IdleSelection], 1.0f);
		GetWorldTimerManager().SetTimer(MontageHandle, this, &AEnemy::DoneIdling, Montage_Length);
	}

}


void AEnemy::CancelIdle() {

	UAnimInstance* instance = GetMesh()->GetAnimInstance();

	if (instance->Montage_IsPlaying(IdleMontages[IdleSelection])) {
		instance->Montage_Stop(1, IdleMontages[IdleSelection]);
	}

	DoneIdling();
}


