// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AILeaderboardWidget.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameStateBase.generated.h"

UCLASS()
class TOYRUNNER_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	AMyGameStateBase();
	
public:
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Round")
	float RoundEndTime;

	UFUNCTION(NetMulticast, Reliable)
	void EndRound();

	UPROPERTY()
	FTimerHandle UpdateTimerHandle;
	
	UFUNCTION(BlueprintCallable, Category = "Round")
	void StartRound(float Duration);

	UFUNCTION()
	void RegisterLeaderboardWidget(UAILeaderboardWidget* NewWidget);

	UFUNCTION()
	void UnregisterLeaderboardWidget(UAILeaderboardWidget* WidgetToRemove);

	UFUNCTION()
	void UpdateAllLeaderboards();

	UFUNCTION(NetMulticast, Reliable)
	void UpdateTime();

	UPROPERTY()
	TArray<UAILeaderboardWidget*> ActiveLeaderboardWidgets;

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Leaderboard")
	TArray<FAILeaderboardEntry> LeaderboardData;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Leaderboard")
	void UpdateLeaderboard(const FString& PlayerName, int32 NewScore);
	
	
};