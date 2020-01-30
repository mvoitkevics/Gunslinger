// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/Shared/GA_RangedAttack.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AT_PlayMontageAndWaitEvent.h"
#include "CharacterBase.h"
#include "GunslingerProjectile.h"
#include "Kismet/KismetMathLibrary.h"

UGA_RangedAttack::UGA_RangedAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Ability1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Skill")));
}


void UGA_RangedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = FireMontage;

	// Play fire montage and wait for event telling us to spawn the projectile
	UAT_PlayMontageAndWaitEvent* Task = UAT_PlayMontageAndWaitEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UGA_RangedAttack::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UGA_RangedAttack::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UGA_RangedAttack::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UGA_RangedAttack::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UGA_RangedAttack::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UGA_RangedAttack::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_RangedAttack::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_RangedAttack::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->Role == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		ACharacterBase* Hero = Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
		if (!Hero)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		FVector Start = Hero->GetGunComponent()->GetSocketLocation(FName("Muzzle"));
		FRotator Dir = Hero->GetControlRotation(); // Hero->GetCameraBoom()->GetComponentLocation() + Hero->GetFollowCamera()->GetForwardVector() * Range;
		//FRotator Rotation = Start. // UKismetMathLibrary::FindLookAtRotation(Start, End);

		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), Hero->GetAttackPower());

		FTransform MuzzleTransform = FTransform(); // Hero->GetGunComponent()->GetSocketTransform(FName("Muzzle"));
		MuzzleTransform.SetRotation(Dir.Quaternion());
		MuzzleTransform.SetLocation(Start);
		MuzzleTransform.SetScale3D(FVector(1.0f));

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		AGunslingerProjectile* Projectile = GetWorld()->SpawnActorDeferred<AGunslingerProjectile>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
			Hero, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
		Projectile->UpgradeGameplayEffects = Hero->UpgradeGameplayEffects;
		Projectile->Range = Hero->GetRange();
		Projectile->FinishSpawning(MuzzleTransform);
	}
}
