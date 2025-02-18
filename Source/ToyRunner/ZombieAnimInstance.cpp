// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAnimInstance.h"

#include "AIController.h"
#include "AIMob.h"
#include "ToyRunnerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UZombieAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    APawn* Pawn = TryGetPawnOwner();
    if (!Pawn) return;  

    Speed = Pawn->GetVelocity().Size();

    AAIController* AIController = Cast<AAIController>(Pawn->GetController());
    if (!AIController) return; 

    UBlackboardComponent* Blackboard = AIController->FindComponentByClass<UBlackboardComponent>();
    if (!Blackboard) return; 
    
    bool bToyThrown = Blackboard->GetValueAsBool(TEXT("ToyThrown"));
    
    if(Speed <= 0.0f && !bToyThrown)
    {
        {
            FindClosestPlayer();
            if (TargetPlayer)
            {
                RotateToTarget(TargetPlayer);
            }
        }
    }
}

void UZombieAnimInstance::FindClosestPlayer()
{
    APawn* OwnerPawn = TryGetPawnOwner();
    if (!OwnerPawn) return;

    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AToyRunnerCharacter::StaticClass(), Players);

    float ClosestDistance = TNumericLimits<float>::Max();
    AActor* ClosestPlayer = nullptr;

    for (AActor* Player : Players)
    {
        AToyRunnerCharacter* Character = Cast<AToyRunnerCharacter>(Player);
        if (Character)
        {
            float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Character->GetActorLocation());

            if (Character->IsHoldingToy())  // Если игрок держит игрушку
            {
                TargetPlayer = Character;
                return;
            }

            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestPlayer = Character;
            }
        }
    }

    TargetPlayer = ClosestPlayer;
}

void UZombieAnimInstance::RotateToTarget(AActor* Target)
{
    if (!Target || !TryGetPawnOwner()) return;

    FRotator LookAtRotation = (Target->GetActorLocation() - TryGetPawnOwner()->GetActorLocation()).Rotation();
    LookAtRotation.Pitch = 0.f;  
    TryGetPawnOwner()->SetActorRotation(FMath::RInterpTo(TryGetPawnOwner()->GetActorRotation(), LookAtRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
}