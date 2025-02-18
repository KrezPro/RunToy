// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "OnlineSubsystem.h"  
#include "OnlineSessionSettings.h" 
#include "ToyRunnerGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h" 
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::Init()
{
	Super::Init();

	SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}

	FString SubsystemName = IOnlineSubsystem::Get()->GetSubsystemName().ToString();

	if (IsRunningDedicatedServer()) // Проверка на Dedicated Server
	{
		HostServer();
	}
	else
	{
		ShowMainMenu();
	}
}

void UMyGameInstance::HostServer()
{
	SessionInterface = IOnlineSubsystem::Get()->GetSessionInterface();
	if (!SessionInterface.IsValid())
	{
		return;
	}
	

	// Проверяем, существует ли уже сессия
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnDestroySessionComplete);
		SessionInterface->DestroySession(NAME_GameSession);
		return;
	}

	// Если сессии нет, создаём новую
	CreateNewSession();
}

// ✅ Вызывается после успешного удаления старой сессии
void UMyGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{

	if (bWasSuccessful)
	{
		// Отложенный вызов создания новой сессии
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMyGameInstance::CreateNewSession, 1.0f, false);
	}
}

// ✅ Функция для создания сессии
void UMyGameInstance::CreateNewSession()
{
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsDedicated = IsRunningDedicatedServer();
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinViaPresence = false; // ❗ NULL не поддерживает Presence
	SessionSettings.NumPublicConnections = 10;
	SessionSettings.bUsesPresence = false;

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
	bool bSessionCreated = SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings);

	if (!bSessionCreated)
	{
		UE_LOG(LogTemp, Error, TEXT("Ошибка: `CreateSession()` вернул false!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("`CreateSession()` вызван успешно, проверяем сессию..."));

		// Проверяем активную сессию
		FNamedOnlineSession* ActiveSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ActiveSession)
		{
			UE_LOG(LogTemp, Warning, TEXT("Найдена активная сессия: %s"), *ActiveSession->SessionName.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Сессия не найдена после создания!"));
		}
	}
}

// ✅ Вызывается при завершении создания сессии
void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		return;
	}

	// Загружаем карту на сервере
	if (IsRunningDedicatedServer())
	{
		GetWorld()->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap?listen", true);
	}

	// ✅ Если это НЕ Dedicated Server (Listen)
	else
	{
		FString CurrentMap = GetWorld()->GetMapName();
		GetWorld()->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap?listen");
	}
}




void UMyGameInstance::OnPlayerJoined()
{
	if (!bIsWaitingForPlayers)
		return;

	NumConnectedPlayers++;
}

void UMyGameInstance::CheckForStart()
{
	if (NumConnectedPlayers >= 1) // Ждём хотя бы одного игрока
	{
		if (GetWorld())
		{
			GetWorld()->ServerTravel("/Game/FirstPerson/Maps/FirstPersonMap?listen");
			bIsWaitingForPlayers = false; // Отключаем ожидание
		}
	}
}

void UMyGameInstance::FindServers()
{
	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (bIsSearchingForServers)
	{
		return;
	}

	bIsSearchingForServers = true;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(FName("SEARCH_PRESENCE"), false, EOnlineComparisonOp::Equals);
	
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UMyGameInstance::StartFindServers()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(FName("SEARCH_PRESENCE"), false, EOnlineComparisonOp::Equals); // Отключаем Presence

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	bIsSearchingForServers = false;

	if (!bWasSuccessful || !SessionSearch.IsValid() || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}
	
	// Логируем найденные сервера
	for (int i = 0; i < SessionSearch->SearchResults.Num(); i++)
	{
		FString ServerName = SessionSearch->SearchResults[i].Session.OwningUserName;
	}

	JoinServer(); // Автоподключение, если нашли сервер
}

void UMyGameInstance::JoinServer()
{
	FString ServerIP = "127.0.0.1:7777";

	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (!SessionSearch || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}

	// Пробуем подключиться через JoinSession
	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
}

bool UMyGameInstance::IsRunningDedicatedServer()
{
	return GIsServer && !GIsClient;
}

void UMyGameInstance::ShowMainMenu()
{
	if (!GEngine)
	{
		return;
	}

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UMyGameInstance::DelayedShowMainMenu, 3.f, false);
}

void UMyGameInstance::DelayedShowMainMenu()
{
	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();

			// Показываем курсор и включаем UI-режим
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (PlayerController)
			{
				PlayerController->bShowMouseCursor = true;
				PlayerController->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void UMyGameInstance::StartReconnectLoop()
{
	if (bIsTryingToReconnect) return;
    
	bIsTryingToReconnect = true;
	FTimerHandle DelayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayTimerHandle, this, &UMyGameInstance::DelayedStartReconnect, 0.5f, false);
}

void UMyGameInstance::DelayedStartReconnect()
{
	if (!SessionInterface.IsValid())
	{
		bIsTryingToReconnect = false;
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(ReconnectTimerHandle, this, &UMyGameInstance::TryReconnect, 0.5f, true);
}

void UMyGameInstance::TryReconnect()
{
	if (!SessionInterface.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(ReconnectTimerHandle);
		bIsTryingToReconnect = false;
		return;
	}

	// Проверяем, есть ли активная сессия
	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReconnectTimerHandle);
		bIsTryingToReconnect = false;
		return;
	}

	FindServers(); // Запускаем поиск серверов

	// Ждём 2 секунды перед выполнением Join, чтобы поиск успел отработать
	FTimerHandle JoinTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(JoinTimerHandle, this, &UMyGameInstance::ReconnectToServer, 0.5f, false);
}

void UMyGameInstance::ReconnectToServer()
{
	FString ServerIP = "127.0.0.1:7777";

	if (!SessionInterface.IsValid())
	{
		return;
	}

	if (!SessionSearch || SessionSearch->SearchResults.Num() == 0)
	{
		return;
	}
	
	// Пробуем подключиться через JoinSession
	SessionInterface->JoinSession(0, NAME_GameSession, SessionSearch->SearchResults[0]);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (Result == EOnJoinSessionCompleteResult::Success)
    {
        FString ConnectString;
        if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
        {
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PlayerController)
            {
                PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
            }

            // Останавливаем автоматический реконнект при успешном подключении
            GetWorld()->GetTimerManager().ClearTimer(ReconnectTimerHandle);
            bIsTryingToReconnect = false;
        }
    }
}

void UMyGameInstance::ExitGame()
{

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->ConsoleCommand(TEXT("quit"));
	}
}

