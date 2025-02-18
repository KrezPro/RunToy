// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "AILeaderboardWidget.generated.h"

USTRUCT(BlueprintType)
struct FAILeaderboardEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	int32 Score;

	FAILeaderboardEntry() : Name(TEXT("Unknown")), Score(0) {}
	FAILeaderboardEntry(FString InName, int32 InScore) : Name(InName), Score(InScore) {}
};

UCLASS()
class TOYRUNNER_API UAILeaderboardWidget : public UUserWidget
{
GENERATED_BODY()
	
	void NativeConstruct();
    void NativeDestruct();

public:
	
	UFUNCTION(BlueprintCallable)
	void UpdateLeaderboard(const TArray<FAILeaderboardEntry>& LeaderboardData);

	UFUNCTION(BlueprintCallable)
	void UpdateRoundTime(float TimeRemaining);

	UPROPERTY()
	TArray<FAILeaderboardEntry> SortedLeaderboardData;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* LeaderboardBox;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundTimeText;

};