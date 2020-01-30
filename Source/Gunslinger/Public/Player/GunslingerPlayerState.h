// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GunslingerPlayerState.generated.h"

class UBaseAttributeSet;
class UGunslingerASC;
/**
 * 
 */
UCLASS()
class GUNSLINGER_API AGunslingerPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
    AGunslingerPlayerState();

    // Implement IAbilitySystemInterface
    class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    class UBaseAttributeSet* GetBaseAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
		bool IsAlive() const;

	/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		int32 GetCharacterLevel() const;

protected:

	UPROPERTY()
		class UGunslingerASC* AbilitySystemComponent;

	UPROPERTY()
		class UBaseAttributeSet* BaseAttributeSet;

	FGameplayTag DeadTag;

	FDelegateHandle HealthChangedDelegateHandle;
	FDelegateHandle MaxHealthChangedDelegateHandle;
	FDelegateHandle XPChangedDelegateHandle;
	FDelegateHandle GoldChangedDelegateHandle;
	FDelegateHandle CharacterLevelChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void MaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void XPChanged(const FOnAttributeChangeData& Data);
	virtual void GoldChanged(const FOnAttributeChangeData& Data);
	virtual void CharacterLevelChanged(const FOnAttributeChangeData& Data);

	// Tag change callbacks
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
};
