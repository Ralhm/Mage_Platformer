// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrapplePoint.generated.h"


UCLASS()
class COMBATGAME_API AGrapplePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrapplePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGrappleBox* Box;

	UFUNCTION(BlueprintCallable)
	void Tug();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeGrappled = true;

};
