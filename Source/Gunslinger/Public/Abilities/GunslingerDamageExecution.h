// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GunslingerDamageExecution.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UGunslingerDamageExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	UGunslingerDamageExecution();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

};
