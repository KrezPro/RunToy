// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API UZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void FindClosestPlayer();
	UFUNCTION()
	void RotateToTarget(AActor* Target);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY()
	AActor* TargetPlayer;
	
};
