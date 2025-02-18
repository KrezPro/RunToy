// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"

#include "AILeaderboardWidget.h"
#include "AIMobController.h"
#include "EngineUtils.h"
#include "MyGameStateBase.h"
#include "ToyRunnerCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (LeaderboardWidgetClass)
	{
		LeaderboardWidget = CreateWidget<UAILeaderboardWidget>(GetWorld(), LeaderboardWidgetClass);
		if (LeaderboardWidget)
		{
			LeaderboardWidget->AddToViewport();

			AMyGameStateBase* MyGameStateCheck = GetWorld()->GetGameState<AMyGameStateBase>();
			if (MyGameStateCheck)
			{
				MyGameStateCheck->RegisterLeaderboardWidget(LeaderboardWidget);
			}
		}
	}
}


void AGameHUD::ClearWidgets()
{
	TArray<UUserWidget*> WidgetsToRemove;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), WidgetsToRemove, UUserWidget::StaticClass(), true);

	for (UUserWidget* Widget : WidgetsToRemove)
	{
		if (Widget)
		{
			Widget->RemoveFromParent(); 
			Widget->ConditionalBeginDestroy();
		}
	}
}

void AGameHUD::ShowWinningWidget()
{
	ClearWidgets();
	
	if (!WinningWidgetClass)
	{
		return;
	}
	
	if (!WinningWidget)
	{
		WinningWidget = CreateWidget<UWinningWindowWidget>(GetWorld(), WinningWidgetClass);
		if (!WinningWidget)
		{
			return;
		}
	}
	
	WinningWidget->AddToViewport(999);
	MyGameState = Cast<AMyGameStateBase>(GetWorld()->GetGameState());
	if (MyGameState)
	{
		WinningWidget->SetWinningInfo(MyGameState->LeaderboardData);
	}
}
