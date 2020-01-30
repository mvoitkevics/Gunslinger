// Fill out your copyright notice in the Description page of Project Settings.


#include "GunslingerAbilityTypes.h"
#include "GunslingerASC.h"
#include "AbilitySystemGlobals.h"

bool FGunslingerGameplayEffectContainerSpec::HasValidEffects() const
{
	return true;
	// return TargetGameplayEffectSpecs.Num() > 0;
}

bool FGunslingerGameplayEffectContainerSpec::HasValidTargets() const
{
	return true;
	// return TargetData.Num() > 0;
}

void FGunslingerGameplayEffectContainerSpec::AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors)
{
	/*for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		TargetData.Add(NewData);
	}*/
}