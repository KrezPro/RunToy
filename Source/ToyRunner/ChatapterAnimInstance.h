// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToyRunnerCharacter.h"
#include "Animation/AnimInstance.h"
#include "ChatapterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API UChatapterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	public:

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	AToyRunnerCharacter* Character;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character Movement")
	UCharacterMovementComponent* MovementComponent;
	
	virtual void NativeInitializeAnimation() override;
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Movement")
	float GroundSpeed;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Movement")
	bool ShouldMove;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool IsAiming;

	UPROPERTY(BlueprintReadWrite, Category = "Character")
	bool PressedJump;
	
	UPROPERTY(BlueprintReadWrite, Category = "Character Movement")
	float Direction;

	UPROPERTY(BlueprintReadWrite, Category = "Character Movement")
	FRotator CharRotation;
};
