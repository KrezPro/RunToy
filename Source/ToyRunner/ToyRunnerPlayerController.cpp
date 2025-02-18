// Copyright Epic Games, Inc. All Rights Reserved.


#include "ToyRunnerPlayerController.h"
#include "ToyRunnerGameMode.h"
#include "Engine/LocalPlayer.h"

void AToyRunnerPlayerController::EndRoundAlready_Implementation()
{
	{
		AToyRunnerGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AToyRunnerGameMode>();
		MyGameMode->RestartServer();
	}
}