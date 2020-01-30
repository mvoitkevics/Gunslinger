// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GunslingerPlayerState.h"
#include "Abilities/BaseAttributeSet.h"
#include "Abilities/GunslingerASC.h"
#include "CharacterBase.h"
#include "GunslingerPlayerController.h"
#include "UI/HealthDisplayWidget.h"
#include "UI/HUDWidget.h"

AGunslingerPlayerState::AGunslingerPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGunslingerASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	BaseAttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("BaseAttributeSet"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag(FName("Character.States.Dead"));
}

UAbilitySystemComponent* AGunslingerPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBaseAttributeSet* AGunslingerPlayerState::GetBaseAttributeSet() const
{
	return BaseAttributeSet;
}

bool AGunslingerPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}


float AGunslingerPlayerState::GetHealth() const
{
	return BaseAttributeSet->GetHealth();
}

float AGunslingerPlayerState::GetMaxHealth() const
{
	return BaseAttributeSet->GetMaxHealth();
}

int32 AGunslingerPlayerState::GetCharacterLevel() const
{
	return BaseAttributeSet->GetCharacterLevel();
}

int32 AGunslingerPlayerState::GetXP() const
{
	return BaseAttributeSet->GetXP();
}

int32 AGunslingerPlayerState::GetGold() const
{
	return BaseAttributeSet->GetGold();
}

void AGunslingerPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetHealthAttribute()).AddUObject(this, &AGunslingerPlayerState::HealthChanged);
		MaxHealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &AGunslingerPlayerState::MaxHealthChanged);
		XPChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetXPAttribute()).AddUObject(this, &AGunslingerPlayerState::XPChanged);
		GoldChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetGoldAttribute()).AddUObject(this, &AGunslingerPlayerState::GoldChanged);
		CharacterLevelChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(BaseAttributeSet->GetCharacterLevelAttribute()).AddUObject(this, &AGunslingerPlayerState::CharacterLevelChanged);

		// Tag change callbacks
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("Character.Effects.Stun")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGunslingerPlayerState::StunTagChanged);
	}
}


void AGunslingerPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;

	// Update floating status bar
	ACharacterBase* Hero = Cast<ACharacterBase>(GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("Players state health change: %f"), Health);
	if (Hero)
	{
		UE_LOG(LogTemp, Warning, TEXT("Players state of: %s"), *Hero->GetName());

		UHealthDisplayWidget* HeroFloatingStatusBar = Hero->GetHealthDisplayWidget();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(Health / GetMaxHealth());
		}
	}

	// Update the HUD
	// Handled in the UI itself using the AsyncTaskAttributeChanged node as an example how to do it in Blueprint

	// If the player died, handle death
	if (!IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Hero)
		{
			Hero->Die();
		}
	}
}

void AGunslingerPlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = Data.NewValue;

	// Update floating status bar
	ACharacterBase* Hero = Cast<ACharacterBase>(GetPawn());
	if (Hero)
	{
		UHealthDisplayWidget* HeroFloatingStatusBar = Hero->GetHealthDisplayWidget();
		if (HeroFloatingStatusBar)
		{
			HeroFloatingStatusBar->SetHealthPercentage(GetHealth() / MaxHealth);
		}
	}

	
}


void AGunslingerPlayerState::XPChanged(const FOnAttributeChangeData& Data)
{
	float NewXP = Data.NewValue;

	// Update the HUD
	AGunslingerPlayerController* PC = Cast<AGunslingerPlayerController>(GetOwner());
	if (PC)
	{
		UHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetExpPercentage(NewXP / 1000.0f);
		}
	}
}

void AGunslingerPlayerState::GoldChanged(const FOnAttributeChangeData& Data)
{
	float NewGold = Data.NewValue;
	// Update the HUD
	AGunslingerPlayerController* PC = Cast<AGunslingerPlayerController>(GetOwner());
	if (PC)
	{
		UHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetGold(NewGold);
		}
	}
}

void AGunslingerPlayerState::CharacterLevelChanged(const FOnAttributeChangeData& Data)
{
	// Do smtng, like pick ability
	int32 NewLevel = (int32) Data.NewValue;

	// Update the HUD
	AGunslingerPlayerController* PC = Cast<AGunslingerPlayerController>(GetOwner());
	if (PC)
	{
		UHUDWidget* HUD = PC->GetHUD();
		if (HUD)
		{
			HUD->SetLevel(NewLevel);
		}
	}
}

void AGunslingerPlayerState::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
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
