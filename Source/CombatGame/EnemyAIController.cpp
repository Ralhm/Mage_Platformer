// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"


#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Perception/AISenseConfig.h"
#include "Perception/AIPerceptionComponent.h"


AEnemyAIController::AEnemyAIController() {
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("SightComponent"));
	TeamId = FGenericTeamId(0); //This teamID stuff is NOT critical to detecting the player

}

void AEnemyAIController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	RunBehaviorTree(Tree);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Possessing Enemy!"));

}

void AEnemyAIController::BeginPlay() {
	Super::BeginPlay();
	UseBlackboard(Tree->BlackboardAsset, Board);
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Controller Spawned!"));

}

ETeamAttitude::Type AEnemyAIController::GetTeamAttitudeTowards(const AActor& Other) const {
	//checks if other actor is a pawn
	auto OtherPawn = Cast<APawn>(&Other);
	if (OtherPawn == nullptr) {
		return ETeamAttitude::Neutral;
	}

	//check if actor (bot or player) implements IGenericTeamAgentInterface.
	auto ActorBot = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()); //ActorBot
	auto ActorPlayer = Cast<IGenericTeamAgentInterface>(&Other); //ActorPlayer
	if (ActorBot == nullptr && ActorPlayer == nullptr) {
		return ETeamAttitude::Neutral;
	}

	if (ActorBot != nullptr) { //print ActorBot TeamID. For Testing only
		FGenericTeamId ActorBotTeamId = ActorBot->GetGenericTeamId();
		int IActorBotTeamId = (int)ActorBotTeamId;
		FString SActorBotTeamId = FString::FromInt(IActorBotTeamId);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Current Light Index: %s"), SActorBotTeamId);
	}

	if (ActorPlayer != nullptr) { //print ActorPlayer TeamID. For Testing only
		FGenericTeamId ActorPlayerTeamId = ActorPlayer->GetGenericTeamId();
		int IActorPlayerTeamId = (int)ActorPlayerTeamId;
		FString SActorPlayerTeamId = FString::FromInt(IActorPlayerTeamId);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, SActorPlayerTeamId);
	}

	//Get the TeamId pf the Actor (bot or Player)
	FGenericTeamId OtherActorTeamId = NULL;
	if (ActorBot != nullptr) {
		OtherActorTeamId = ActorBot->GetGenericTeamId();
	}
	else if (ActorPlayer != nullptr) {
		OtherActorTeamId = ActorPlayer->GetGenericTeamId();
	}

	//Determines ThisBot attitude towards the OtherActor (Bot or Player) as either neutral, friendly or hostile
	FGenericTeamId ThisBotTeamId = this->GetGenericTeamId();
	if (OtherActorTeamId == 255) { //They are not on a team
		return ETeamAttitude::Neutral;
	}
	else if (ThisBotTeamId == OtherActorTeamId) { //They are on the same team
		return ETeamAttitude::Friendly;
	}
	else { //They are on a different team
		return ETeamAttitude::Hostile;
	}

}
