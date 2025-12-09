// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleBox.h"

#include "Components/BoxComponent.h"
#include "Components/InterpToMovementComponent.h"

#include "GrapplePoint.h"

// Sets default values
AGrappleBox::AGrappleBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OriginComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TugBoxSceneComponent"));

	RootComponent = OriginComponent;



	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	GrapplePointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrapplePointMesh"));

	MeshComponent->SetupAttachment(OriginComponent);
	GrapplePointMesh->SetupAttachment(MeshComponent);

}

// Called when the game starts or when spawned
void AGrappleBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrappleBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Moving) {

		SetActorLocation(FMath::InterpEaseOut(GetActorLocation(), GrapplePoint, DeltaTime, LerpRate), false);

		if ((GetActorLocation() - GrapplePoint).Size() < 10) {
			Moving = false;
		}
	}



}

void AGrappleBox::Tug() {

	Moving = true;

}


void AGrappleBox::TugTimer() {




	//MeshComponent->AddWorldOffset();

	//MeshComponent->AddLocalOffset(FMath::Lerp(MeshComponent->GetComponentLocation(), TugPosition, LerpRate));

	//MeshComponent->SetWorldLocation(FMath::Lerp(MeshComponent->GetComponentLocation(), GrappleEndPointMesh->GetComponentLocation(), LerpRate));





}

void AGrappleBox::SetTugPosition(FVector Pos) {
	GrapplePoint = Pos;
}