// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class COMBATGAME_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	
public:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTree* Tree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBlackboardComponent* Board;

	FGenericTeamId TeamId;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

};
