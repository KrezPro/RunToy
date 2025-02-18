#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIMob.h"
#include "AIMobController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class TOYRUNNER_API AAIMobController : public AAIController
{
	GENERATED_BODY()

public:
	AAIMobController();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UBlackboardData* BlackboardAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComponent;

	UFUNCTION()
	void SetToyLocation(FVector Location); 
	
public:
	UFUNCTION()
	void AddScore(int32 Points);

	UFUNCTION()
	int32 GetScore() const { return Score; }  // Функция для UI
	
	UFUNCTION(Server, Reliable)
	void Server_UpdateLeader(AAIMobController* NewLeader);

	UPROPERTY()
	class AToyRunnerCharacter* MyCharacter;
	
	static AAIMobController* CurrentLeader;

	UPROPERTY(BlueprintReadOnly, Category = "Score") 
	int32 Score = 0;
};