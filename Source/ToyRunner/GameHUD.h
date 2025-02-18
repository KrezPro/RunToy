// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WinningWindowWidget.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API AGameHUD : public AHUD
{
	GENERATED_BODY()
private:

public:
    virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ClearWidgets();
	

	UFUNCTION(BlueprintCallable)
	void ShowWinningWidget();

	/** Указатель на виджет победы */
	UPROPERTY(EditAnywhere)
	UWinningWindowWidget* WinningWidget;

	UPROPERTY()
	class AMyGameStateBase* MyGameState;

	UPROPERTY()
	TArray<FAILeaderboardEntry> SortedLeaderboardData;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UWinningWindowWidget> WinningWidgetClass;
	
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UAILeaderboardWidget> LeaderboardWidgetClass;

    UPROPERTY()
    UAILeaderboardWidget* LeaderboardWidget;
};
