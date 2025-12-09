// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleBox.generated.h"

UCLASS()
class COMBATGAME_API AGrappleBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrappleBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USceneComponent* OriginComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UStaticMeshComponent* GrapplePointMesh;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector GrapplePoint;

	UFUNCTION(BlueprintCallable)
	void Tug();


	UFUNCTION(BlueprintCallable)
	void SetTugPosition(FVector Pos);


	UFUNCTION()
	void TugTimer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeGrappled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LerpRate = 0.01f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Moving;

	FTimerHandle LerpHandle;

};
