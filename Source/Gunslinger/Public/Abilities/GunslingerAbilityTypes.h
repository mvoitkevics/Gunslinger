// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GunslingerAbilityTypes.generated.h"

class UGunslingerASC;
class UGameplayEffect;
class UGunslingerTargetType;

/**
 * Struct defining a list of gameplay effects, a tag, and targeting info
 * These containers are defined statically in blueprints or assets and then turn into Specs at runtime
 */
USTRUCT(BlueprintType)
struct FGunslingerGameplayEffectContainer
{
	GENERATED_BODY()

public:
	FGunslingerGameplayEffectContainer() {}

	/** Sets the way that targeting happens */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TSubclassOf<class UGunslingerTargetType> TargetType;

	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TArray<TSubclassOf<UGameplayEffect>> TargetGameplayEffectClasses;
};

/** A "processed" version of FGunslingerGameplayEffectContainer that can be passed around and eventually applied */
USTRUCT(BlueprintType)
struct FGunslingerGameplayEffectContainerSpec
{
	GENERATED_BODY()

public:

	FGunslingerGameplayEffectContainerSpec() {}


	/** List of gameplay effects to apply to the targets */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		TArray<FGameplayEffectSpecHandle> TargetGameplayEffectSpecs;

	/** Computed target data */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GameplayEffectContainer)
		FGameplayAbilityTargetDataHandle TargetData;

	/** Returns true if this has any valid effect specs */
	bool HasValidEffects() const;

	/** Returns true if this has any valid targets */
	bool HasValidTargets() const;

	/** Adds new targets to target data */
	void AddTargets(const TArray<FHitResult>& HitResults, const TArray<AActor*>& TargetActors);
};