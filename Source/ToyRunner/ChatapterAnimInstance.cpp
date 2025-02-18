// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatapterAnimInstance.h"
#include "Animation/UMGSequencePlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

void UChatapterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    APawn* OwningPawn = TryGetPawnOwner();
    if (OwningPawn)
    {
        Character = Cast<AToyRunnerCharacter>(OwningPawn);
        if (Character)
        {
            MovementComponent = Character->GetCharacterMovement();
        }
    }
}

void UChatapterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    
    if (!Character || !MovementComponent)
    {
        return;
    }
    
    FVector Velocity = Character->GetVelocity();
    float VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
    GroundSpeed = VelocityXY;
    
    FVector CurrentAcceleration = MovementComponent->GetCurrentAcceleration();
    bool bIsAccelerating = !CurrentAcceleration.IsNearlyZero();
    ShouldMove = (GroundSpeed > 3.0f) && bIsAccelerating;
    
    FRotator BaseRotation = Character->GetActorRotation();
    FRotator VelocityRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
    Direction = UKismetMathLibrary::NormalizedDeltaRotator(VelocityRotation, BaseRotation).Yaw;
    
    FRotator ActorRotation = Character->GetActorRotation();
    
    FRotator ControlRotation = Character->GetControlRotation();

    IsAiming = Character->bIsAiming;
    
    FRotator DeltaRotation = ActorRotation - ControlRotation;
    
    CharRotation = FRotator(0.0f, 0.0f, DeltaRotation.Pitch);
    
}