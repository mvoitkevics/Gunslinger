// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GunslingerASC.h"
#include "Abilities/BaseAttributeSet.h"
#include "CharacterBase.generated.h"

class UGunslingerGameplayAbility;
class UGameplayEffect;
class UHealthDisplayWidget;
class ADamageIndicator;
class UWidgetComponent;
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterBaseHitReactDelegate, EGDHitReactDirection, Direction);


UCLASS()
class GUNSLINGER_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual bool IsAlive() const;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		EStates CurrentState = EStates::Stopped;

	
	USkeletalMeshComponent* GetGunComponent() const;

	/**
	* Getters for attributes from GDAttributeSetBase
	**/
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetRange() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetAttackPower() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetAttackSpeed() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Upgrades")
		TArray<TSubclassOf<UGameplayEffect>> UpgradeGameplayEffects;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	FGameplayTag DeadTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	FGameplayTag EffectRemoveOnDeathTag;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* DeathMontage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		USkeletalMeshComponent* GunComponent;

	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
		int32 CharacterLevel;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<UGunslingerGameplayAbility>> GameplayAbilities;

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** The component used to handle ability system interactions */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
		UGunslingerASC* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
		UBaseAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
		UDataTable* AttrDataTable;

	/** If true we have initialized our abilities */
	UPROPERTY()
		int32 bAbilitiesInitialized;


	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Called from BaseAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);


	/** Apply the startup gameplay abilities and effects */
	void AddStartupGameplayAbilities();

	/** Attempts to remove any startup gameplay abilities */
	void RemoveStartupGameplayAbilities();

	/**
	 * Attempts to activate all abilities that match the specified tags
	 * Returns true if it thinks it activated, but it may return false positives due to failure later in activation.
	 * If bAllowRemoteActivation is true, it will remotely activate local/server abilities, if false it will only try to locally activate the ability
	 */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	/** Returns a list of active abilities matching the specified tags. This only returns if the ability is currently running */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UGunslingerGameplayAbility*>& ActiveAbilities);

	/** Returns total time and remaining time for cooldown tags. Returns false if no active cooldowns found */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration);


	// Friended to allow access to handle functions above
	friend UBaseAttributeSet;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UHealthDisplayWidget> HealthDisplayWidgetClass;

	UPROPERTY()
		class UHealthDisplayWidget* HealthDisplayWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
		class UWidgetComponent* HealthWidgetComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
		TSubclassOf<class ADamageIndicator> DamageIndicatorClass;

	// Creates and initializes the floating status bar for heroes.
	// Safe to call many times because it checks to make sure it only executes once.
	UFUNCTION()
		void InitializeFloatingStatusBar();

	// Client only
	virtual void OnRep_PlayerState() override;

public:	
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void FinishDying();


	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
		ACharacterBase* CurrentTarget = nullptr;

	class UHealthDisplayWidget* GetHealthDisplayWidget();

	// Send Events to Blueprints

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void BP_PerformAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void BP_EndAttack();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void BP_MoveStart(const FVector Destination);

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void BP_MoveEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement")
	void BP_Scan();
};
