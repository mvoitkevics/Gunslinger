// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitReceiveDamage.generated.h"

class UGunslingerASC;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWaitReceiveDamageDelegate, class UGunslingerASC*, SourceASC, float, Damage);

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UAT_WaitReceiveDamage : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FWaitReceiveDamageDelegate OnDamage;

	virtual void Activate() override;

	UFUNCTION()
		void OnDamageReceived(class UGunslingerASC* SourceASC, float Damage);

	// Wait until the ability owner receives damage.
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_WaitReceiveDamage* WaitReceiveDamage(UGameplayAbility* OwningAbility, bool TriggerOnce);

protected:
	bool TriggerOnce;

	virtual void OnDestroy(bool AbilityIsEnding) override;
};
