#include "AIMobController.h"

#include "AIMob.h"
#include "EngineUtils.h"
#include "GameHUD.h"
#include "MyGameStateBase.h"
#include "ToyRunnerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Net/UnrealNetwork.h"

AAIMobController* AAIMobController::CurrentLeader = nullptr;

AAIMobController::AAIMobController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
}

void AAIMobController::BeginPlay()
{
	Super::BeginPlay();

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{

		if (BlackboardAsset)
		{
			UseBlackboard(BlackboardAsset, BlackboardComponent);
		}

		if (BehaviorTree)
		{
			RunBehaviorTree(BehaviorTree);
		}
	}
}


void AAIMobController::SetToyLocation(FVector Location)
{
	if (!BlackboardComponent) return;

	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AAIMobController*> AIControllers;
	TArray<float> Distances;

	// Собираем всех мобов и их дистанции до игрушки
	for (TActorIterator<AAIMobController> It(World); It; ++It)
	{
		AAIMobController* AIController = *It;
		if (AIController && AIController->GetPawn())
		{
			float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), Location);
			AIControllers.Add(AIController);
			Distances.Add(Distance);
		}
	}
	
	if (AIControllers.Num() == 0) return;

	// Определяем количество мобов, которые НЕ пойдут (половина от общего количества)
	int32 MobsToStay = AIControllers.Num() / 2;

	// Создаём массив индексов, сортируем его по дальности до игрушки
	TArray<int32> SortedIndices;
	for (int32 i = 0; i < AIControllers.Num(); i++)
	{
		SortedIndices.Add(i);
	}

	SortedIndices.Sort([&](int32 A, int32 B) { return Distances[A] > Distances[B]; });

	// Оставляем минимум одного моба, который всегда пойдёт
	if (MobsToStay >= AIControllers.Num()) MobsToStay = AIControllers.Num() - 1;

	// Делаем так, чтобы дальние мобы НЕ шли к игрушке
	for (int32 i = 0; i < MobsToStay; i++)
	{
		AIControllers[SortedIndices[i]]->BlackboardComponent->SetValueAsBool(TEXT("ShouldChaseToy"), false);
	}

	// Остальным мобам назначаем цель
	for (int32 i = MobsToStay; i < AIControllers.Num(); i++)
	{
		AIControllers[SortedIndices[i]]->BlackboardComponent->SetValueAsVector(TEXT("ToyLocation"), Location);
		AIControllers[SortedIndices[i]]->BlackboardComponent->SetValueAsBool(TEXT("ShouldChaseToy"), true);
	}
}

void AAIMobController::AddScore(int32 Points)
{
	Score += Points;

	// Если это первый моб или если новый лидер
	if (!CurrentLeader || Score > CurrentLeader->Score)
	{
		Server_UpdateLeader(this);
	}

	AMyGameStateBase* MyGameState = GetWorld()->GetGameState<AMyGameStateBase>();
	if (MyGameState)
	{
		AAIMob* CurrentMob = Cast<AAIMob>(GetPawn()); // Проверяем, действительно ли это AAIMob
		if (CurrentMob)
		{
			MyGameState->UpdateLeaderboard(CurrentMob->MobName, Score);
		}
	}
}

void AAIMobController::Server_UpdateLeader_Implementation(AAIMobController* NewLeader)
{
	if (!NewLeader)
	{
		return;
	}
	
	if (CurrentLeader && !CurrentLeader->IsValidLowLevel())
	{
		CurrentLeader = nullptr;
	}

	// Получаем мобов с проверкой на `nullptr`
	AAIMob* OldLeaderMob = (CurrentLeader && CurrentLeader->GetPawn()) ? Cast<AAIMob>(CurrentLeader->GetPawn()) : nullptr;
	AAIMob* NewLeaderMob = (NewLeader && NewLeader->GetPawn()) ? Cast<AAIMob>(NewLeader->GetPawn()) : nullptr;

	// Проверяем, есть ли `NewLeaderMob`
	if (!NewLeaderMob || !NewLeaderMob->IsValidLowLevel())
	{
		return;
	}

	// Если есть старый лидер, снимаем статус лидера
	if (OldLeaderMob)
	{
		OldLeaderMob->bIsLeader = false;
		OldLeaderMob->OnRep_LeaderStatus(); // Принудительное обновление
	}
	
	NewLeaderMob->bIsLeader = true;
	NewLeaderMob->OnRep_LeaderStatus();
	
	CurrentLeader = NewLeader;
}