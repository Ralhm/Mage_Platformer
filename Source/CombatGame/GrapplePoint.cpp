// Fill out your copyright notice in the Description page of Project Settings.


#include "GrapplePoint.h"


#include "GrappleBox.h"

// Sets default values
AGrapplePoint::AGrapplePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;


}

// Called when the game starts or when spawned
void AGrapplePoint::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AGrapplePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

void AGrapplePoint::Tug() {
	
	Box->SetTugPosition(GetActorLocation());
	Box->Tug();
}