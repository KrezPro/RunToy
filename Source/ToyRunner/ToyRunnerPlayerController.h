// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ToyRunnerPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class TOYRUNNER_API AToyRunnerPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Game")
	void EndRoundAlready();
	
};
