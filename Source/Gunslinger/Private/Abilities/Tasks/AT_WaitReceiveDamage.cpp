// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Tasks/AT_WaitReceiveDamage.h"
#include "Abilities/GunslingerASC.h"


UAT_WaitReceiveDamage::UAT_WaitReceiveDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TriggerOnce = false;
}

UAT_WaitReceiveDamage* UAT_WaitReceiveDamage::WaitReceiveDamage(UGameplayAbility* OwningAbility, bool InTriggerOnce)
{
	UAT_WaitReceiveDamage* MyObj = NewAbilityTask<UAT_WaitReceiveDamage>(OwningAbility);
	MyObj->TriggerOnce = InTriggerOnce;
	return MyObj;
}

void UAT_WaitReceiveDamage::Activate()
{
	UGunslingerASC* GDASC = Cast<UGunslingerASC>(AbilitySystemComponent);

	if (GDASC)
	{
		GDASC->ReceivedDamage.AddDynamic(this, &UAT_WaitReceiveDamage::OnDamageReceived);
	}
}

void UAT_WaitReceiveDamage::OnDestroy(bool AbilityIsEnding)
{
	UGunslingerASC* GDASC = Cast<UGunslingerASC>(AbilitySystemComponent);

	if (GDASC)
	{
		GDASC->ReceivedDamage.RemoveDynamic(this, &UAT_WaitReceiveDamage::OnDamageReceived);
	}

	Super::OnDestroy(AbilityIsEnding);
}

void UAT_WaitReceiveDamage::OnDamageReceived(UGunslingerASC* SourceASC, float Damage)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnDamage.Broadcast(SourceASC, Damage);
	}

	if (TriggerOnce)
	{
		EndTask();
	}
}
