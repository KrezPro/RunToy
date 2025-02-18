// Fill out your copyright notice in the Description page of Project Settings.


#include "WinningWindowWidget.h"
#include "Components/TextBlock.h"

void UWinningWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UWinningWindowWidget::SetWinningInfo(const TArray<FAILeaderboardEntry>& LeaderboardData)
{
	if (LeaderboardData.Num() > 0)
	{
		int32 MaxScore = LeaderboardData[0].Score;
		TArray<FString> WinnerEntries;

		for (const FAILeaderboardEntry& Entry : LeaderboardData)
		{
			if (Entry.Score == MaxScore)
			{
				WinnerEntries.Add(FString::Printf(TEXT("%s - %d"), *Entry.Name, Entry.Score));
			}
			else
			{
				break; 
			}
		}

		FString WinnerName;
		bool bIsTie = WinnerEntries.Num() > 1;

		if (bIsTie)
		{
			WinnerName = TEXT("Tie:\n") + FString::Join(WinnerEntries, TEXT("\n"));
		}
		else
		{
			WinnerName = TEXT("Winner:\n") + FString::Join(WinnerEntries[0],TEXT(""));
		}

		if (VictoryText)
		{
			VictoryText->SetText(FText::FromString(WinnerName));
		}
	}
}