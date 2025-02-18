// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TextRenderComponent.h"
#include "AIMob.generated.h"

UCLASS()
class TOYRUNNER_API AAIMob : public ACharacter
{
	GENERATED_BODY()

public:
	AAIMob();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void MoveToToy(FVector ToyLocation); 

	UFUNCTION()
	void OnToyPickedUp(); 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* NameTag; 

	UPROPERTY(ReplicatedUsing = OnRep_LeaderStatus, Replicated)
	bool bIsLeader = false;

	UFUNCTION()
	void OnRep_LeaderStatus();

	UPROPERTY(ReplicatedUsing = OnRep_MobName, Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Mob Info", meta = (AllowPrivateAccess = "true"))
	FString MobName; 

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	
	UFUNCTION()
	void AssignRandomName(); 

	UFUNCTION()
	void OnRep_MobName();

};
