// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/**
	* Attribute setters
	*/
	/*UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetMaxHealth(float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetCurrentHealth(float CurrentHealth);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetHealthPercentage(float HealthPercentage);*/

	// ----------------------------------------------------- //

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetLevel(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetGold(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetExpPercentage(float ExpPercentage);
};
