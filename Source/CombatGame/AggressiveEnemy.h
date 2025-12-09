// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "AggressiveEnemy.generated.h"

/**
 * 
 */
UCLASS()
class COMBATGAME_API AAggressiveEnemy : public AEnemy
{
	GENERATED_BODY()
	

public:
	// Sets default values for this character's properties
	AAggressiveEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
