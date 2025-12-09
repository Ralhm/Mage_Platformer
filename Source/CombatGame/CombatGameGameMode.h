// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CombatGameGameMode.generated.h"

UCLASS(minimalapi)
class ACombatGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACombatGameGameMode();

public:


	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class TSubclassOf<UUserWidget> UIWidget;

	UPROPERTY()
	class UUserWidget* UIDisplay;


	//The widget instance we are using as our game menu
	UPROPERTY()
	class UUserWidget* CurrentWidget;


	UFUNCTION(BlueprintCallable)
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);
};



