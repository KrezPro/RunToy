// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnerBase.h"

ASpawnerBase::ASpawnerBase()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawnerBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector ASpawnerBase::GetRandomSpawnPoint()
{
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnerBase: Нет точек спауна!"));
		return GetActorLocation();
	}

	FVector RandomPoint = SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)];

	// Луч вниз для определения высоты
	FVector Start = RandomPoint + FVector(0, 0, 500);
	FVector End = RandomPoint - FVector(0, 0, 1000);
    
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.bTraceComplex = true;
	Params.bReturnPhysicalMaterial = false;
    
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		return HitResult.ImpactPoint;
	}

	return RandomPoint;
}

