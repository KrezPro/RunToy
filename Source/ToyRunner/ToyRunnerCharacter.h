// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MyToy.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ToyRunnerCharacter.generated.h"

UCLASS()
class AToyRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AToyRunnerCharacter();
	
	UFUNCTION(BlueprintCallable)
	bool IsHoldingToy() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toy")
	TSubclassOf<AMyToy> CubeToyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Toy")
	TSubclassOf<AMyToy> SphereToyClass;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void PickupToy(AMyToy* Toy);
	
	UFUNCTION(Server,Reliable)
	void ThrowToy();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<AMyToy>> ToyClassesToSpawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated, Category="Battle Royale Cource")
	bool bIsAiming;

	UPROPERTY(Replicated)
	bool bToyAvailableForThrow = true;

protected:
	
	virtual void BeginPlay() override;

	// Настройка привязки ввода
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Функции управления движением
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	UFUNCTION()
	void StartJump();
	UFUNCTION()
	void StopJump();

	// Камера от первого лица
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCameraComponent;

	// Текущий выбранный тип игрушки (Cube или Sphere)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Toy")
	EToyType CurrentToyType;

	// Флаг, указывающий, что игрушка уже находится в игре (брошена и ещё не подобрана)
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Toy")
	bool bToyActive;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Toy")
	AMyToy* CurrentToyActor = nullptr;
};
