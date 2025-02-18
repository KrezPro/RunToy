// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyToy.generated.h"
//Общие данные для ToyRunnerCharacter и MyToy

UENUM(BlueprintType)
enum class EToyType : uint8
{
	Cube   UMETA(DisplayName = "Cube"),
	Sphere UMETA(DisplayName = "Sphere")
};

UCLASS()
class TOYRUNNER_API AMyToy : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyToy();
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnPickupTimeout();

	UPROPERTY()
	FTimerHandle PickupTimeoutHandle;

	// Вызывается при старте игры
	virtual void BeginPlay() override;

	// Функция репликации свойств
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Тип игрушки (определяет начисляемые очки)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Toy")
	EToyType ToyType;

	// Значение очков, начисляемых при подборе (Cube = 1, Sphere = 3)
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Toy")
	int32 ScoreValue;

	// Компонент для отображения меша и коллизии
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

protected:

	// Обработчик начала пересечения (Overlap)
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                    bool bFromSweep, const FHitResult& SweepResult);
	
};
