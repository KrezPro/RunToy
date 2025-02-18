// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"
#include "GameHUD.h"
#include "ToyRunnerCharacter.h"
#include "Net/UnrealNetwork.h"

AMyGameStateBase::AMyGameStateBase()
{
	bReplicates = true;
}

void AMyGameStateBase::StartRound(float Duration)
{
	if (HasAuthority()) 
	{
	
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if(!TimerManager.IsTimerActive(UpdateTimerHandle))
		{
		RoundEndTime = GetWorld()->GetTimeSeconds() + Duration;
			
			GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &AMyGameStateBase::UpdateTime, 1.0f, true, 0.0f);
		}
	}
}

void AMyGameStateBase::UpdateTime_Implementation()
{
	//Расчитываем время окончания раунда для клиентов
	if(HasAuthority())
	{
		RoundEndTime -= 1;
	}
	for (UAILeaderboardWidget* Widget : ActiveLeaderboardWidgets)
	{
		if (Widget)
		{
			Widget->UpdateRoundTime(RoundEndTime);
		}
	}
	
	if (RoundEndTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		EndRound();
	}
}

void AMyGameStateBase::EndRound_Implementation()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());

		AHUD* BaseHUD = PC->GetHUD();
		AGameHUD* HUD = Cast<AGameHUD>(BaseHUD);
		if (HUD)
		{
			HUD->ShowWinningWidget(); 
		}
	}
}

void AMyGameStateBase::UpdateLeaderboard_Implementation(const FString& PlayerName, int32 NewScore)
{
	bool bPlayerExists = false;

	// Проверяем, есть ли уже такой игрок в таблице лидеров
	for (FAILeaderboardEntry& Entry : LeaderboardData)
	{
		if (Entry.Name == PlayerName)
		{
			Entry.Score = NewScore;
			bPlayerExists = true;
			break;
		}
	}

	// Если игрока нет, добавляем нового
	if (!bPlayerExists)
	{
		LeaderboardData.Add({ PlayerName, NewScore });
	}

	// Сортируем список по убыванию очков
	LeaderboardData.Sort([](const FAILeaderboardEntry& A, const FAILeaderboardEntry& B)
	{
		return A.Score > B.Score;
	});
	UpdateAllLeaderboards();
}


void AMyGameStateBase::RegisterLeaderboardWidget(UAILeaderboardWidget* NewWidget)
{
	if (!NewWidget || ActiveLeaderboardWidgets.Contains(NewWidget))
		return;

	ActiveLeaderboardWidgets.Add(NewWidget);
	
	UpdateAllLeaderboards();
}

void AMyGameStateBase::UnregisterLeaderboardWidget(UAILeaderboardWidget* WidgetToRemove)
{
	if (!WidgetToRemove)
		return;

	ActiveLeaderboardWidgets.Remove(WidgetToRemove);
}

void AMyGameStateBase::UpdateAllLeaderboards()
{
	for (UAILeaderboardWidget* Widget : ActiveLeaderboardWidgets)
	{
		if (Widget)
		{
			Widget->UpdateLeaderboard(LeaderboardData); // Метод обновления UI
		}
	}
}

void AMyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyGameStateBase, LeaderboardData);
	DOREPLIFETIME(AMyGameStateBase, RoundEndTime);
}

