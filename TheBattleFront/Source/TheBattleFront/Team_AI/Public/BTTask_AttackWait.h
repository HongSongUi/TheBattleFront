// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TheBattleFront.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackWait.generated.h"

/**
 * 
 */
UCLASS()
class THEBATTLEFRONT_API UBTTask_AttackWait : public UBTTaskNode
{
	GENERATED_BODY()
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
protected:
private:
	float CurrentDelay;
};
