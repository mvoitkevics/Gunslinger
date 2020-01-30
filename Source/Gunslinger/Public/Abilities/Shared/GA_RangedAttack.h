// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GunslingerGameplayAbility.h"
#include "GA_RangedAttack.generated.h"

class AGunslingerProjectile;

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UGA_RangedAttack : public UGunslingerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_RangedAttack();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		UAnimMontage* FireMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSubclassOf<AGunslingerProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSubclassOf<UGameplayEffect> DamageGameplayEffect;

	/** Actually activate ability, do not call this directly. We'll call it from APAHeroCharacter::ActivateAbilitiesWithTags(). */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UFUNCTION()
		void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
		void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
		void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);
};
