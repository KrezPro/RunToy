// Fill out your copyright notice in the Description page of Project Settings.


#include "AILeaderboardWidget.h"
#include "MyGameStateBase.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"

void UAILeaderboardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAILeaderboardWidget::NativeDestruct()
{
	Super::NativeDestruct();

	AMyGameStateBase* MyGameState = GetWorld()->GetGameState<AMyGameStateBase>();
	if (MyGameState)
	{
		MyGameState->UnregisterLeaderboardWidget(this);
	}
}

void UAILeaderboardWidget::UpdateLeaderboard(const TArray<FAILeaderboardEntry>& LeaderboardData)
{
	if (!LeaderboardBox) return;

	// Очищаем старые данные
	LeaderboardBox->ClearChildren();

	// Создаем копию данных для сортировки
	SortedLeaderboardData = LeaderboardData;

	// Сортируем данные по убыванию очков
	SortedLeaderboardData.Sort([](const FAILeaderboardEntry& A, const FAILeaderboardEntry& B)
	{
		return A.Score > B.Score;
	});

	// Находим максимальное количество очков (лидера)
	int32 MaxScore = SortedLeaderboardData.Num() > 0 ? SortedLeaderboardData[0].Score : 0;

	// Создаём новый список
	for (const FAILeaderboardEntry& Entry : SortedLeaderboardData)
	{
		UTextBlock* EntryText = NewObject<UTextBlock>(this);
		if (!EntryText) continue;

		EntryText->SetText(FText::FromString(FString::Printf(TEXT("%s - %d"), *Entry.Name, Entry.Score)));

		// Красный цвет для всех, кроме лидера
		EntryText->SetColorAndOpacity(FSlateColor(Entry.Score == MaxScore ? FLinearColor::Green : FLinearColor::Red));

		UVerticalBoxSlot* LeaderboardSlot = LeaderboardBox->AddChildToVerticalBox(EntryText);
		if (LeaderboardSlot)
		{
			LeaderboardSlot->SetPadding(FMargin(5.0f));
		}
	}
}

void UAILeaderboardWidget::UpdateRoundTime(float TimeRemaining)
{
	//Синхронизируем клиентом с хостом на 1 тик таймера
	if (GetWorld()->GetAuthGameMode() == nullptr)
	{
		TimeRemaining -= 1;
	}
	if (!RoundTimeText) return;

	int32 Minutes = FMath::FloorToInt((TimeRemaining) / 60.0f);
	int32 Seconds = FMath::RoundToInt(TimeRemaining) % 60;
	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	RoundTimeText->SetText(FText::FromString(TimeString));
}