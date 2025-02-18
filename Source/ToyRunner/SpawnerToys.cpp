// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerToys.h"
#include "MyToy.h"

ASpawnerToys::ASpawnerToys()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnerToys::SpawnToys(int32 NumToys)
{
	if (!ToyClassCommon || !ToyClassRare)
	{
		return;
	}

	for (int32 i = 0; i < NumToys; i++)
	{
		bool bIsRare = FMath::RandRange(0, 100) < 10; // 10% шанс редкой игрушки
		TSubclassOf<AActor> ToyToSpawn = bIsRare ? ToyClassRare : ToyClassCommon;

		FVector SpawnLocation = GetRandomSpawnPoint();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		if (ToyToSpawn)
		{
			// Указываем параметры спауна
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Спауним игрушку
			AMyToy* SpawnedToy = GetWorld()->SpawnActor<AMyToy>(ToyToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedToy)
			{
				// Отключаем Hit Events у StaticMeshComponent
				UStaticMeshComponent* MeshComponent = SpawnedToy->FindComponentByClass<UStaticMeshComponent>();
				if (MeshComponent)
				{
					MeshComponent->SetNotifyRigidBodyCollision(false); 
				}
			}
		}
		
	}
}

void ASpawnerToys::SpawnSingleToy()
{
	if (!ToyClassCommon || !ToyClassRare)
	{
		return;
	}

	bool bIsRare = FMath::RandRange(0, 100) < 10; // 10% шанс редкой игрушки
	TSubclassOf<AActor> ToyToSpawn = bIsRare ? ToyClassRare : ToyClassCommon;

	FVector SpawnLocation = GetRandomSpawnPoint();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (ToyToSpawn)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Спауним игрушку
		AMyToy* SpawnedToy = GetWorld()->SpawnActor<AMyToy>(ToyToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
		if (SpawnedToy)
		{
			// Отключаем Hit Events у StaticMeshComponent
			UStaticMeshComponent* MeshComponent = SpawnedToy->FindComponentByClass<UStaticMeshComponent>();
			if (MeshComponent)
			{
				MeshComponent->SetNotifyRigidBodyCollision(false); // Это отключает Hit Events
			}
		}
	}
}