// Fill out your copyright notice in the Description page of Project Settings.


#include "UBTTask_MoveToToy.h"
#include "BehaviorTree/BlackboardComponent.h" 
#include "AIMobController.h"

UBTTask_MoveToToy::UBTTask_MoveToToy()
{
	NodeName = "Move To Toy";
}

EBTNodeResult::Type UBTTask_MoveToToy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIMobController* AIController = Cast<AAIMobController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_MoveToToy`: AIController == nullptr!"));
		return EBTNodeResult::Failed;
	}

	FVector ToyLocation = AIController->BlackboardComponent->GetValueAsVector(TEXT("ToyLocation"));
    
	if (ToyLocation.IsZero())
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_MoveToToy`: ToyLocation == (0,0,0)!"));
		return EBTNodeResult::Failed;
	}

	AIController->MoveToLocation(ToyLocation);

	return EBTNodeResult::Succeeded;
}