// Fill out your copyright notice in the Description page of Project Settings.


#include "GunslingerASC.h"
#include "GunslingerCharacter.h"
#include "GunslingerGameplayAbility.h"
#include "AbilitySystemGlobals.h"


UGunslingerASC::UGunslingerASC() {}


void UGunslingerASC::GetActiveAbilitiesWithTags(const FGameplayTagContainer& GameplayTagContainer, TArray<UGunslingerGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);

	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			ActiveAbilities.Add(Cast<UGunslingerGameplayAbility>(ActiveAbility));
		}
	}
}

int32 UGunslingerASC::GetDefaultAbilityLevel() const
{
	AGunslingerCharacter* OwningCharacter = Cast<AGunslingerCharacter>(OwnerActor);

	if (OwningCharacter)
	{
		return 1; // OwningCharacter->GetCharacterLevel();
	}
	return 1;
}

UGunslingerASC* UGunslingerASC::GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	return Cast<UGunslingerASC>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}

void UGunslingerASC::ReceiveDamage(UGunslingerASC* SourceASC, float Damage)
{
	ReceivedDamage.Broadcast(SourceASC, Damage);
}