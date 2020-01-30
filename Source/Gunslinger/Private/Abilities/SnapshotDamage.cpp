// Fill out your copyright notice in the Description page of Project Settings.


#include "SnapshotDamage.h"
#include "Abilities/BaseAttributeSet.h"
#include "Abilities/GunslingerASC.h"

struct GunslingerDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);

	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PoisonDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BurnDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ElectricDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(Damage);

	GunslingerDamageStatics()
	{
		// Capture the Target's DefensePower attribute. Do not snapshot it, because we want to use the health value at the moment we apply the execution.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, DefensePower, Target, false);

		// Capture the Source's AttackPower. We do want to snapshot this at the moment we create the GameplayEffectSpec that will execute the damage.
		// (imagine we fire a projectile: we create the GE Spec when the projectile is fired. When it hits the target, we want to use the AttackPower at the moment
		// the projectile was launched, not when it hits).
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, PoisonDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, BurnDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, ElectricDamage, Source, false);

		// Also capture the source's raw Damage, which is normally passed in directly via the execution
		DEFINE_ATTRIBUTE_CAPTUREDEF(UBaseAttributeSet, Damage, Source, true);
	}
};

static const GunslingerDamageStatics& DamageStatics()
{
	static GunslingerDamageStatics DmgStatics;
	return DmgStatics;
}

USnapshotDamage::USnapshotDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().DefensePowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().PoisonDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().BurnDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().ElectricDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void USnapshotDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->AvatarActor : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->AvatarActor : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Gather the tags from the source and target as that can affect which buffs should be used
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// --------------------------------------
	//	Damage Done = Damage * AttackPower / DefensePower
	//	If DefensePower is 0, it is treated as 1.0
	// --------------------------------------

	float Damage = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

	float DefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefensePowerDef, EvaluationParameters, DefensePower);

	float AttackPower = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);

	Damage = AttackPower;

	float PoisonDamage = 1.f;
	// if poison tag
	if (SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Poison"))))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().PoisonDamageDef, EvaluationParameters, PoisonDamage);

		Damage *= PoisonDamage;
	}

	float BurnDamage = 1.f;
	// if poison tag
	if (SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Burn"))))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BurnDamageDef, EvaluationParameters, BurnDamage);
		Damage *= BurnDamage;
	}

	float ElectricDamage = 1.f;
	// if poison tag
	if (SourceTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Data.Damage.Electric"))))
	{
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ElectricDamageDef, EvaluationParameters, ElectricDamage);
		Damage *= ElectricDamage;
	}

	// float Damage = (AttackPower * PoisonDamage * BurnDamage * ElectricDamage) - DefensePower;
	// ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DamageDef, EvaluationParameters, Damage);

	float DamageDone = FMath::RoundHalfToEven(Damage - DefensePower);

	UE_LOG(LogTemp, Warning, TEXT("Total Damage: %f"), DamageDone);
	if (DamageDone != 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().DamageProperty, EGameplayModOp::Additive, DamageDone));
	}
}