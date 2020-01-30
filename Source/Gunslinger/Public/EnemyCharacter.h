// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "EnemyCharacter.generated.h"

class UGunslingerASC;
class UBaseAttributeSet;
class UHealthDisplayWidget;

/**
 * 
 */
UCLASS()
class GUNSLINGER_API AEnemyCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
    AEnemyCharacter();

protected:

	virtual void BeginPlay() override;

	FDelegateHandle HealthChangedDelegateHandle;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

};
