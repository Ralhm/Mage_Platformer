// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatGameGameMode.h"
#include "CombatGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"

ACombatGameGameMode::ACombatGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ACombatGameGameMode::BeginPlay() {
	Super::BeginPlay();
	ChangeMenuWidget(UIWidget);

	UIDisplay = CreateWidget<UUserWidget>(GetWorld(), UIWidget);
	UIDisplay->AddToViewport();
}

void ACombatGameGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass) {
	if (CurrentWidget != nullptr) {
		CurrentWidget->RemoveFromViewport();
		CurrentWidget = nullptr;
	}

	if (NewWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}
}
