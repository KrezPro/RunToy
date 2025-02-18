// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerBase.generated.h"

UCLASS()
class TOYRUNNER_API ASpawnerBase : public AActor
{
	GENERATED_BODY()
	
public:
	ASpawnerBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TArray<FVector> SpawnPoints; 

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	FVector GetRandomSpawnPoint();

protected:
	virtual void BeginPlay() override;

};
