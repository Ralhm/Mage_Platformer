// Fill out your copyright notice in the Description page of Project Settings.


#include "AggressiveEnemy.h"

#include "BehaviorTree/BlackboardComponent.h"

#include "Components/CapsuleComponent.h"

#include "EnemyAIController.h"


#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"


AAggressiveEnemy::AAggressiveEnemy() {
	PrimaryActorTick.bCanEverTick = true;
	TeamId = FGenericTeamId(0);


}


// Called every frame
void AAggressiveEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAggressiveEnemy::BeginPlay() {

	Super::BeginPlay();

	TargetPlayer = UGameplayStatics::GetActorOfClass(GetWorld(), Target);
	CanAttack = true;

	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::Detect);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Found Controller!")));

	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("No Controller Found! Spawning Controller")));
		SpawnDefaultController();
		AIController = Cast<AEnemyAIController>(GetController());
		AIController->PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemy::Detect);
	}
}