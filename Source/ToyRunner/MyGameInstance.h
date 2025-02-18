// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	UPROPERTY()
	FTimerHandle ReconnectTimerHandle;
	UPROPERTY()
	bool bIsTryingToReconnect = false;  // Флаг для избежания дублирования запросов

public:
	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartReconnectLoop();  
	UFUNCTION()
	void DelayedStartReconnect();
    UFUNCTION()
	void TryReconnect(); 
    UPROPERTY()
	bool bIsSearchingForServers;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category = "Network")
	void HostServer();
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinServer();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void FindServers();
	
	UFUNCTION()
	void StartFindServers();

	UPROPERTY()
	UUserWidget* MainMenuWidget;
	
	UFUNCTION(BlueprintCallable, Category = "Game")
	void ExitGame();

	UPROPERTY()
	int32 CurrentRestartAttempt = 1;
	UPROPERTY()
	int32 MaxRestartAttempts = 10;
	UPROPERTY()
	FTimerHandle RestartGameTimerHandle;
	
private:
	
	IOnlineSessionPtr SessionInterface;
	
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	UFUNCTION()
	void Init();
	
	bool IsRunningDedicatedServer();

	UFUNCTION()
	void ShowMainMenu();
	void DelayedShowMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void ReconnectToServer();

	void CreateNewSession();
	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	public:
	UFUNCTION()
		void OnPlayerJoined(); 
	UFUNCTION()
		void CheckForStart(); 


	private:
	UPROPERTY()
	//Логика не закончена!!!
		int32 NumConnectedPlayers = 0; // Количество подключенных игроков
	UPROPERTY()
		bool bIsWaitingForPlayers = false; // Флаг ожидания игроков
	UPROPERTY()
		FTimerHandle StartGameTimerHandle; // Таймер проверки
};
