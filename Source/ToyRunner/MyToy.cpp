// Fill out your copyright notice in the Description page of Project Settings.


#include "MyToy.h"
#include "EngineUtils.h"
#include "AIMobController.h"
#include "SpawnerToys.h"
#include "ToyRunnerCharacter.h"
#include "Net/UnrealNetwork.h"


AMyToy::AMyToy()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Создаем компонент меша и назначаем его корневым
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Включаем физику и настраиваем коллизию
	MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECC_PhysicsBody);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	MeshComponent->SetNotifyRigidBodyCollision(true);

	MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	MeshComponent->OnComponentHit.AddDynamic(this, &AMyToy::OnHit);
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AMyToy::OnOverlapBegin);
	
	// По умолчанию: тип куб и 1 очко, или (если тип Sphere) 3 очка
	ToyType = EToyType::Cube;
	ScoreValue = (ToyType == EToyType::Cube) ? 1 : 3;
}

void AMyToy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
				   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) 
	{
		return;
	}
	
	FString OtherActorName = OtherActor->GetName();

	// Проверяем, есть ли мир
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	// Проверяем, есть ли AI-контроллеры
	bool bFoundAI = false;
	GetWorld()->GetTimerManager().ClearTimer(PickupTimeoutHandle);
	for (TActorIterator<AAIMobController> It(World); It; ++It)
	{
		AAIMobController* AIController = *It;
		if (AIController)
		{
			AIController->SetToyLocation(Hit.Location);
			bFoundAI = true;
		}
	}
	//Назначаем таймер на то что если мобы не смогут добраться до игрушки или другие баги
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (!TimerManager.IsTimerActive(PickupTimeoutHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(PickupTimeoutHandle, this, &AMyToy::OnPickupTimeout, 10.0f, false);
	}
}

void AMyToy::OnPickupTimeout()
{
	// Если игрушка всё еще существует (не была подобрана)
	if (IsValid(this))
	{
		UWorld* World = GetWorld();
		if (World)
		{
			for (TActorIterator<AToyRunnerCharacter> It(GetWorld()); It; ++It)
			{
				It->bToyAvailableForThrow = true;
			}
		}
	}
}

void AMyToy::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
}

void AMyToy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                            bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor != this)
    {

        // Если OtherActor — это игрок, пробуем выполнить подбор
        AToyRunnerCharacter* Character = Cast<AToyRunnerCharacter>(OtherActor);
        if (Character)
        {
            if (HasAuthority())
            {
                Character->PickupToy(this);
            }
        }
        else
        {
            // Если игрушку подобрал моб
            AAIMobController* AIController = Cast<AAIMobController>(OtherActor->GetInstigatorController());
            if (AIController && HasAuthority())
            {

                // Проверяем тип игрушки перед начислением очков
                if (ToyType == EToyType::Cube)
                {
                    ScoreValue = 1;
                }
                else if (ToyType == EToyType::Sphere)
                {
                    ScoreValue = 3;
                }
                else
                {
                    return;
                }

            	ASpawnerToys* Spawner = nullptr;
            	for (TActorIterator<ASpawnerToys> It(GetWorld()); It; ++It)
            	{
            		Spawner = *It;
            		break; // Берём первый найденный спаунер
            	}

            	if (Spawner)
            	{
            		UE_LOG(LogTemp, Warning, TEXT("Спауним новую игрушку!"));
            		Spawner->SpawnSingleToy();
            	}

                // Лог начисления очков
            	for (TActorIterator<AToyRunnerCharacter> It(GetWorld()); It; ++It)
            	{
            		It->bToyAvailableForThrow = true;
            	}
                // Начисляем очки мобу
                AIController->AddScore(ScoreValue);

                // Удаляем игрушку после подбора
                Destroy();
            }
        }
    }
}


void AMyToy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyToy, ToyType);
	DOREPLIFETIME(AMyToy, ScoreValue);
}


