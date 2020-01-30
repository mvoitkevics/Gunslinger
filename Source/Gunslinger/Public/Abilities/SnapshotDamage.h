// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "SnapshotDamage.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API USnapshotDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	USnapshotDamage();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
