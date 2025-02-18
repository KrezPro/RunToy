// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AILeaderboardWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "WinningWindowWidget.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API UWinningWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	/** Обновить информацию о победителе. Если WinnerName пустой, выводится "Ничья". */
	UFUNCTION(BlueprintCallable, Category = "Winning")
	void SetWinningInfo(const TArray<FAILeaderboardEntry>& LeaderboardData);

	UPROPERTY(meta = (BindWidget))
	UTextBlock* VictoryText;

protected:
	virtual void NativeConstruct() override;

	// Кнопка "Начать заново".
	UPROPERTY(meta = (BindWidget))
	UButton* RestartButton;

	// Кнопка "Выйти".
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};