// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "AbilitySystemGlobals.h"
#include "Abilities/GunslingerGameplayAbility.h"
#include "WidgetComponent.h"
#include "UI/DamageIndicator.h"
#include "UI/HealthDisplayWidget.h"
#include "UI/HUDWidget.h"
#include "GunslingerPlayerController.h"
#include "GunslingerGameMode.h"
#include "Player/GunslingerPlayerState.h"

// Sets default values
ACharacterBase::ACharacterBase(): Super()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGunslingerASC>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Create the attribute set, this replicates by default
	AttributeSet = CreateDefaultSubobject<UBaseAttributeSet>(TEXT("AttributeSet"));

	GunComponent = CreateDefaultSubobject<USkeletalMeshComponent>(FName("Gun"));
	GunComponent->SetupAttachment(GetMesh(), FName("WeaponSocket"));

	DeadTag = FGameplayTag::RequestGameplayTag(FName("Character.States.Dead"));
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag(FName("Character.Effects.RemoveOnDeath"));

	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("HealthWidgetComponent"));
	HealthWidgetComponent->SetupAttachment(GetRootComponent());
	HealthWidgetComponent->SetRelativeLocation(FVector(0, 0, 120));
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetDrawSize(FVector2D(500, 500));
	if (HealthDisplayWidgetClass)
	{
		HealthWidgetComponent->SetWidgetClass(HealthDisplayWidgetClass);
	}
	
	CharacterLevel = 1;
	bAbilitiesInitialized = false;
}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

USkeletalMeshComponent* ACharacterBase::GetGunComponent() const
{
	return GunComponent;
}

void ACharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	// Tell Game Mode we died
	if (AGunslingerGameMode* GM = Cast<AGunslingerGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GM->OnCharacterDied.Broadcast(this);
	}


	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}
	// do smtng
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void ACharacterBase::FinishDying()
{
	Destroy();
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UHealthDisplayWidget* ACharacterBase::GetHealthDisplayWidget()
{
	return HealthDisplayWidget;
}

void ACharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo, const FGameplayTagContainer& DamageTags, ACharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	// spawn damage widget
	
	FTransform NewTransform = GetTransform();

	NewTransform.SetLocation(GetActorLocation());
	NewTransform.SetRotation(FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f).Quaternion());
	NewTransform.SetScale3D({ 1, 1 ,1 });

	ADamageIndicator* DeferredChild = GetWorld()->SpawnActorDeferred<ADamageIndicator>(
		DamageIndicatorClass.Get(),
		NewTransform,
		this, InstigatorCharacter,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	DeferredChild->DamageReceived = DamageAmount;

	// pass tags, to understand what was the damage type
	DeferredChild->bIsCriticalDamage = false;


	UGameplayStatics::FinishSpawningActor(DeferredChild, NewTransform);

	// send to blueprints if needed
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void ACharacterBase::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		// update health widget
		UHealthDisplayWidget* HWidget = GetHealthDisplayWidget();
		if (HWidget)
		{
			HWidget->SetHealthPercentage(GetHealth() / GetMaxHealth());
		} else {
			UE_LOG(LogTemp, Warning, TEXT("No Health Widget"));
		}

		// send to blueprints if needed
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void ACharacterBase::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UGunslingerGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			// always give level 1 abilities
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, 1, INDEX_NONE, this));
		}

		// Now apply passives
		for (TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		// This are item abilities
		// AddSlottedGameplayAbilities();

		bAbilitiesInitialized = true;
	}
}

void ACharacterBase::RemoveStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (Role == ROLE_Authority && bAbilitiesInitialized)
	{
		// Remove any abilities added from a previous call
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && GameplayAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		// Do in two passes so the removal happens after we have the full list
		for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
		{
			AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		}

		// Remove all of the passive gameplay effects that were applied by this character
		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);

		// RemoveSlottedGameplayAbilities(true);

		bAbilitiesInitialized = false;
	}
}

bool ACharacterBase::ActivateAbilitiesWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

void ACharacterBase::GetActiveAbilitiesWithTags(FGameplayTagContainer AbilityTags, TArray<UGunslingerGameplayAbility*>& ActiveAbilities)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetActiveAbilitiesWithTags(AbilityTags, ActiveAbilities);
	}
}

bool ACharacterBase::GetCooldownRemainingForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration)
{
	if (AbilitySystemComponent && CooldownTags.Num() > 0)
	{
		TimeRemaining = 0.f;
		CooldownDuration = 0.f;

		FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);
		TArray< TPair<float, float> > DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);
		if (DurationAndTimeRemaining.Num() > 0)
		{
			int32 BestIdx = 0;
			float LongestTime = DurationAndTimeRemaining[0].Key;
			for (int32 Idx = 1; Idx < DurationAndTimeRemaining.Num(); ++Idx)
			{
				if (DurationAndTimeRemaining[Idx].Key > LongestTime)
				{
					LongestTime = DurationAndTimeRemaining[Idx].Key;
					BestIdx = Idx;
				}
			}

			TimeRemaining = DurationAndTimeRemaining[BestIdx].Key;
			CooldownDuration = DurationAndTimeRemaining[BestIdx].Value;

			return true;
		}
	}
	return false;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ACharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}


int32 ACharacterBase::GetCharacterLevel() const
{
	if (AttributeSet)
	{
		return static_cast<int32>(AttributeSet->GetCharacterLevel());
	}

	return 0;
}

float ACharacterBase::GetHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetMaxHealth() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetMaxHealth();
	}

	return 0.0f;
}

float ACharacterBase::GetRange() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetRange();
	}

	return 0.0f;
}

float ACharacterBase::GetAttackPower() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetAttackPower();
	}

	return 0.0f;
}

float ACharacterBase::GetAttackSpeed() const
{
	if (AttributeSet)
	{
		return AttributeSet->GetAttackSpeed();
	}

	return 0.5f;
}


void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Try setting the inventory source, this will fail for AI
	/* InventorySource = NewController;

	if (InventorySource)
	{
		InventoryUpdateHandle = InventorySource->GetSlottedItemChangedDelegate().AddUObject(this, &ACharacterBase::OnItemSlotChanged);
		InventoryLoadedHandle = InventorySource->GetInventoryLoadedDelegate().AddUObject(this, &ACharacterBase::RefreshSlottedGameplayAbilities);
	}*/

	// Initialize our abilities
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (AbilitySystemComponent && AttrDataTable) {
			// const UAttributeSet* Attrs = AbilitySystemComponent->InitStats(UBaseAttributeSet::StaticClass(), AttrDataTable);
		}

		AddStartupGameplayAbilities();
	}

	InitializeFloatingStatusBar();
}

void ACharacterBase::InitializeFloatingStatusBar()
{
	// Only create once
	if (HealthDisplayWidget || !AbilitySystemComponent)
	{
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AGunslingerPlayerController* PC = Cast<AGunslingerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC && PC->IsLocalPlayerController())
	{
		if (HealthDisplayWidgetClass)
		{
			
			HealthDisplayWidget = CreateWidget<UHealthDisplayWidget>(PC, HealthDisplayWidgetClass);
			if (HealthDisplayWidget && HealthWidgetComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("Creating Widget"));
				// Setup the floating status bar
				HealthDisplayWidget->SetHealthPercentage(GetHealth() / GetMaxHealth());
				HealthWidgetComponent->SetWidget(HealthDisplayWidget);
			}
		}
	}
}
void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AGunslingerPlayerState* PS = GetPlayerState<AGunslingerPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		// AbilitySystemComponent = Cast<UGunslingerASC>(PS->GetAbilitySystemComponent());

		// Refresh ASC Actor Info for clients. Server will be refreshed by its AI/PlayerController when it possesses a new Actor.
		// AbilitySystemComponent->RefreshAbilityActorInfo();

		// Set the AttributeSetBase for convenience attribute functions
		// AttributeSet = PS->GetBaseAttributeSet();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		// InitializeAttributes();

		AGunslingerPlayerController* PC = Cast<AGunslingerPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
		InitializeFloatingStatusBar();

		// Respawn specific things that won't affect first possession.

		// Forcibly set the DeadTag count to 0
		AbilitySystemComponent->SetTagMapCount(DeadTag, 0);

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		// SetHealth(GetMaxHealth());
		// SetMana(GetMaxMana());
		// SetStamina(GetMaxStamina());
	}

}
void ACharacterBase::UnPossessed()
{
	// Unmap from inventory source
	/*if (InventorySource && InventoryUpdateHandle.IsValid())
	{
		InventorySource->GetSlottedItemChangedDelegate().Remove(InventoryUpdateHandle);
		InventoryUpdateHandle.Reset();

		InventorySource->GetInventoryLoadedDelegate().Remove(InventoryLoadedHandle);
		InventoryLoadedHandle.Reset();
	}

	InventorySource = nullptr;*/
}

void ACharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		// AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, CharacterLevel);
}
