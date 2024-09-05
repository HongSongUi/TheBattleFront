// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TheBattleFront.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack_Boss.generated.h"

/**
 * 
 */
UCLASS()
class THEBATTLEFRONT_API UBTTask_Attack_Boss : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_Attack_Boss();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:

private:
};
