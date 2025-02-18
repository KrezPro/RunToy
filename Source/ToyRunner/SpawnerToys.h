// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnerBase.h"
#include "SpawnerToys.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API ASpawnerToys : public ASpawnerBase
{
	GENERATED_BODY()
	
public:
	ASpawnerToys();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> ToyClassCommon; // Обычная игрушка

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AActor> ToyClassRare; // Редкая игрушка

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnToys(int32 NumToys);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnSingleToy();
};
