// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAttributeSet.h"
#include "CharacterBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

UBaseAttributeSet::UBaseAttributeSet()
	: Health(1.f)
	, MaxHealth(1.f)
	, AttackPower(1.0f)
	, DefensePower(1.0f)
	, AttackSpeed(.20f)
	, CritChance(1.0f)
	, CritMulti(1.0f)
	, LifeSteal(0.f)
	, SteampackHealing(50.f)
	, Dodge(0.0f)
	, XP(0.0f)
	, Gold(0.0f)
	, Range(700.0f)
	, CharacterLevel(1)
	, CharacterLevelPersistent(1)
	, Damage(0.0f)
{
}

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseAttributeSet, Health);
	DOREPLIFETIME(UBaseAttributeSet, MaxHealth);
	DOREPLIFETIME(UBaseAttributeSet, AttackPower);
	DOREPLIFETIME(UBaseAttributeSet, DefensePower);
	DOREPLIFETIME(UBaseAttributeSet, AttackSpeed);
	DOREPLIFETIME(UBaseAttributeSet, CritChance);
	DOREPLIFETIME(UBaseAttributeSet, CritMulti);
	DOREPLIFETIME(UBaseAttributeSet, LifeSteal);
	DOREPLIFETIME(UBaseAttributeSet, SteampackHealing);
	DOREPLIFETIME(UBaseAttributeSet, Dodge);
	DOREPLIFETIME(UBaseAttributeSet, XP);
	DOREPLIFETIME(UBaseAttributeSet, Gold);
	DOREPLIFETIME(UBaseAttributeSet, Range);
	DOREPLIFETIME(UBaseAttributeSet, CharacterLevel);
	DOREPLIFETIME(UBaseAttributeSet, CharacterLevelPersistent);

}


void UBaseAttributeSet::OnRep_Health()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Health);
}

void UBaseAttributeSet::OnRep_MaxHealth()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, MaxHealth);
}


void UBaseAttributeSet::OnRep_AttackPower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, AttackPower);
}

void UBaseAttributeSet::OnRep_DefensePower()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, DefensePower);
}

void UBaseAttributeSet::OnRep_AttackSpeed()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, AttackSpeed);
}

void UBaseAttributeSet::OnRep_CritChance()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, CritChance);
}

void UBaseAttributeSet::OnRep_CritMulti()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, CritMulti);
}

void UBaseAttributeSet::OnRep_LifeSteal()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, LifeSteal);
}
void UBaseAttributeSet::OnRep_SteampackHealing()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, SteampackHealing);
}
void UBaseAttributeSet::OnRep_Dodge()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Dodge);
}

void UBaseAttributeSet::OnRep_XP()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, XP);
}
void UBaseAttributeSet::OnRep_Gold()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Gold);
}

void UBaseAttributeSet::OnRep_Range()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, Range);
}

void UBaseAttributeSet::OnRep_CharacterLevel()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, CharacterLevel);
}

void UBaseAttributeSet::OnRep_CharacterLevelPersistent()
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBaseAttributeSet, CharacterLevelPersistent);
}

void UBaseAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// UE_LOG(LogTemp, Warning, TEXT("%s Attribute Change"), *Attribute.AttributeName);
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	/*else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}*/
}

void UBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();

	// Compute the delta between old and new, if it is available
	float DeltaValue = 0;
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive)
	{
		// If this was additive, store the raw delta value to be passed along later
		DeltaValue = Data.EvaluatedData.Magnitude;
	}

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ACharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ACharacterBase>(TargetActor);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Get the Source actor
		AActor* SourceActor = nullptr;
		AController* SourceController = nullptr;
		ACharacterBase* SourceCharacter = nullptr;
		if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
		{
			SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
			SourceController = Source->AbilityActorInfo->PlayerController.Get();
			if (SourceController == nullptr && SourceActor != nullptr)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			// Use the controller to find the source pawn
			if (SourceController)
			{
				SourceCharacter = Cast<ACharacterBase>(SourceController->GetPawn());
			}
			else
			{
				SourceCharacter = Cast<ACharacterBase>(SourceActor);
			}

			// Set the causer actor based on context if it's set
			if (Context.GetEffectCauser())
			{
				SourceActor = Context.GetEffectCauser();
			}
		}

		// Try to extract a hit result
		FHitResult HitResult;
		if (Context.GetHitResult())
		{
			HitResult = *Context.GetHitResult();
		}

		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// Apply the health change and then clamp it
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamageDone, 0.0f, GetMaxHealth()));

			if (TargetCharacter)
			{
				// This is proper damage
				TargetCharacter->HandleDamage(LocalDamageDone, HitResult, SourceTags, SourceCharacter, SourceActor);

				// Call for all health changes
				TargetCharacter->HandleHealthChanged(-LocalDamageDone, SourceTags);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes such as from healing or direct modifiers
		// First clamp it
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

		if (TargetCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("Health changed: %f"), DeltaValue);
			// Call for all health changes
			TargetCharacter->HandleHealthChanged(DeltaValue, SourceTags);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCharacterLevelAttribute())
	{
		// DO Upgrade Stuff
	}
	//else if (Data.EvaluatedData.Attribute == GetRangeAttribute())
	//{
	//	// Clamp mana
	//	SetRange(FMath::Clamp(DeltaValue, 0.0f, 3.0f));
	//}
	//else if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	//{
	//	if (TargetCharacter)
	//	{
	//		// Call for all movespeed changes
	//		// TargetCharacter->HandleMoveSpeedChanged(DeltaValue, SourceTags);
	//	}
	//}
}
