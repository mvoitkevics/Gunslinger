// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Abilities/GunslingerASC.h"
#include "Abilities/BaseAttributeSet.h"
#include "WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/HealthDisplayWidget.h"

AEnemyCharacter::AEnemyCharacter() : Super()
{

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		// InitializeAttributes();
		AddStartupGameplayAbilities();

		InitializeFloatingStatusBar();

		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AEnemyCharacter::HealthChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Character.Effects.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemyCharacter::StunTagChanged);
	}
}

void AEnemyCharacter::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	if (HealthDisplayWidget)
	{
		HealthDisplayWidget->SetHealthPercentage(Health / GetMaxHealth());
	}

	// If the minion died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		Die();
	}
}

void AEnemyCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		FGameplayTagContainer AbilityTagsToCancel;
		AbilityTagsToCancel.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability")));

		FGameplayTagContainer AbilityTagsToIgnore;
		AbilityTagsToIgnore.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.NotCanceledByStun")));

		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel, &AbilityTagsToIgnore);
	}
}