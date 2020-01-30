// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BaseAttributeSet.generated.h"


// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * Base Attribute Set For All Living creatures
 */
UCLASS()
class GUNSLINGER_API UBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UBaseAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Current Health, when 0 we expect owner to die. Capped by MaxHealth */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Health)

	/** MaxHealth is its own attribute, since GameplayEffects may modify it */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, MaxHealth)

	/** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, AttackPower)

	/** Base Damage is divided by DefensePower to get actual damage done, so 1.0 means no bonus */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
		FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, DefensePower)

	/** Attack Speed defines how quickly the character attacks */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackSpeed)
		FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, AttackSpeed)

	/** Crit Chance is how often do you crit */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_CritChance)
		FGameplayAttributeData CritChance;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CritChance)

	/** Crit Multi is the strength of Critical hit */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_CritMulti)
		FGameplayAttributeData CritMulti;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CritMulti)

	/** % of Healing on Kill */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_LifeSteal)
		FGameplayAttributeData LifeSteal;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, LifeSteal)

	/** Healing effect of Steampacks */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_SteampackHealing)
		FGameplayAttributeData SteampackHealing;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, SteampackHealing)

	/** Dodge % */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Dodge)
		FGameplayAttributeData Dodge;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Dodge)

	/** Dodge % */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_XP)
		FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, XP)

	/** Dodge % */
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Gold)
		FGameplayAttributeData Gold;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Gold)

	/** Range % */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_Range)
		FGameplayAttributeData Range;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Range)

	/** Range % */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_PoisonDamage)
		FGameplayAttributeData PoisonDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, PoisonDamage)

	/** Range % */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_BurnDamage)
		FGameplayAttributeData BurnDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, BurnDamage)

	/** Range % */
	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_ElectricDamage)
		FGameplayAttributeData ElectricDamage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, ElectricDamage)

	/** Level During Runs */
	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_CharacterLevel)
		FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CharacterLevel)
	
	/** Profile Level */
	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_CharacterLevelPersistent)
		FGameplayAttributeData CharacterLevelPersistent;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, CharacterLevelPersistent)

	/** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into -Health */
	UPROPERTY(BlueprintReadOnly, Category = "Mana", meta = (HideFromLevelInfos))
		FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UBaseAttributeSet, Damage)


protected:
	/** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes. (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	// These OnRep functions exist to make sure that the ability system internal representations are synchronized properly during replication
	UFUNCTION()
		virtual void OnRep_Health();

	UFUNCTION()
		virtual void OnRep_MaxHealth();

	UFUNCTION()
		virtual void OnRep_AttackPower();

	UFUNCTION()
		virtual void OnRep_DefensePower();

	UFUNCTION()
		virtual void OnRep_AttackSpeed();

	UFUNCTION()
		virtual void OnRep_CritChance();

	UFUNCTION()
		virtual void OnRep_CritMulti();

	UFUNCTION()
		virtual void OnRep_LifeSteal();

	UFUNCTION()
		virtual void OnRep_SteampackHealing();

	UFUNCTION()
		virtual void OnRep_Dodge();

	UFUNCTION()
		virtual void OnRep_XP();

	UFUNCTION()
		virtual void OnRep_Gold();

	UFUNCTION()
		virtual void OnRep_Range();

	UFUNCTION()
		virtual void OnRep_PoisonDamage();

	UFUNCTION()
		virtual void OnRep_BurnDamage();

	UFUNCTION()
		virtual void OnRep_ElectricDamage();

	UFUNCTION()
		virtual void OnRep_CharacterLevel();

	UFUNCTION()
		virtual void OnRep_CharacterLevelPersistent();

};
