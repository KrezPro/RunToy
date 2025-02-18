// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIMobController.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTask_MoveToToy.generated.h"

/**
 * 
 */
UCLASS()
class TOYRUNNER_API UBTTask_MoveToToy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MoveToToy();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
