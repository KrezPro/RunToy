// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameHUD.h"
#include "WinningWindowWidget.h"
#include "GameFramework/GameModeBase.h"
#include "ToyRunnerGameMode.generated.h"

UCLASS(minimalapi)
class AToyRunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AToyRunnerGameMode();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Round")
	float RoundTime = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 NumMobsToSpawn = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 NumToysToSpawn = 5;

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RestartServer();
	
};



