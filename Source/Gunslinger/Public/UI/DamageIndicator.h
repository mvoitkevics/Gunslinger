// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemGlobals.h"
#include "DamageIndicator.generated.h"

class UDamageWidgetComponent;
class UProjectileMovementComponent;

UCLASS()
class GUNSLINGER_API ADamageIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageIndicator(const class FObjectInitializer& PCIP);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage)
		float DamageReceived = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Damage)
		FGameplayTagContainer DamageTags;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UDamageWidgetComponent* DamageWidget;

	UPROPERTY(EditDefaultsOnly, Category = Damage)
		float DamageIndicatorLifeSpan = 1.2f;

};
