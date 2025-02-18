// Copyright Epic Games, Inc. All Rights Reserved.

#include "ToyRunnerGameMode.h"

#include "EngineUtils.h"
#include "MyGameInstance.h"
#include "MyGameStateBase.h"
#include "OnlineSubsystem.h"
#include "SpawnerToys.h"
#include "ToyRunnerPlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AToyRunnerGameMode::AToyRunnerGameMode()
	: Super()
{
	PlayerControllerClass = AToyRunnerPlayerController::StaticClass();
}

void AToyRunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	AMyGameStateBase* MyGameState = GetGameState<AMyGameStateBase>();
	if (MyGameState)
	{
		// Устанавливаем время завершения раунда (например, 15 секунд)
		MyGameState->StartRound(RoundTime);
	}
	
	TArray<ASpawnerToys*> ToySpawners;

	for (TActorIterator<ASpawnerToys> It(GetWorld()); It; ++It)
	{
		ToySpawners.Add(*It);
	}
	
	// Распределяем игрушки по спаунерам
	for (int32 i = 0; i < NumToysToSpawn; i++)
	{
		if (ToySpawners.Num() > 0)
		{
			ToySpawners[FMath::RandRange(0, ToySpawners.Num() - 1)]->SpawnToys(1);
		}
	}
}

void AToyRunnerGameMode::RestartServer_Implementation()
{
    // Останавливаем все таймеры для текущего объекта (GameMode)
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    // Удаляем все виджеты из Viewport, чтобы избежать утечек
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->RemoveAllViewportWidgets();
    }

    // Принудительная сборка мусора
    CollectGarbage(RF_NoFlags);

    // Удаляем текущую сессию
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        if (SessionInterface.IsValid() && SessionInterface->GetNamedSession(NAME_GameSession))
        {
            SessionInterface->DestroySession(NAME_GameSession);
        }
    }

    // Получаем GameInstance и вызываем метод, который заставит клиентов начать поиск сессии
    UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GI)
    {
        GI->HostServer();
    }
}
