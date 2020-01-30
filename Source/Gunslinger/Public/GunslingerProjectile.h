// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "GunslingerProjectile.generated.h"

UCLASS()
class GUNSLINGER_API AGunslingerProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunslingerProjectile();

	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		FGameplayEffectSpecHandle DamageEffectSpecHandle;

	// Passed down from Player
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
		TArray<TSubclassOf<UGameplayEffect>> UpgradeGameplayEffects;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (ExposeOnSpawn = true))
		float Range;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
