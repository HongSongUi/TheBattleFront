// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TheBattleFront.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToPatrolRoute.generated.h"

/**
 * 
 */
UCLASS()
class THEBATTLEFRONT_API UBTTask_MoveToPatrolRoute : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_MoveToPatrolRoute();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	//virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
