// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthDisplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UHealthDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetHealthPercentage(float HealthPercentage);
};
