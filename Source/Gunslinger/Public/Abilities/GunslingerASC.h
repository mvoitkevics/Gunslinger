// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "AbilitySystemComponent.h"
#include "GunslingerASC.generated.h"

class UGunslingerGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReceivedDamageDelegate, UGunslingerASC*, SourceASC, float, Damage);

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UGunslingerASC : public UAbilitySystemComponent
{
	GENERATED_BODY()
	

public:
	// Constructors and overrides
	UGunslingerASC();

	/** Returns a list of currently active ability instances that match the tags */
	void GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGunslingerGameplayAbility*>& ActiveAbilities);

	/** Returns the default level used for ability activations, derived from the character */
	int32 GetDefaultAbilityLevel() const;

	/** Version of function in AbilitySystemGlobals that returns correct type */
	static UGunslingerASC* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);

	FReceivedDamageDelegate ReceivedDamage;

	// Called from GDDamageExecCalculation. Broadcasts on ReceivedDamage whenever this ASC receives damage.
	virtual void ReceiveDamage(UGunslingerASC* SourceASC, float Damage);

};
