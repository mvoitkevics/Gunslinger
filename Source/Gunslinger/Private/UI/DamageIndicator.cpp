// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageIndicator.h"
#include "UI/DamageWidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADamageIndicator::ADamageIndicator(const class FObjectInitializer& PCIP)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = PCIP.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootComponent"));

	ProjectileMovement = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = FMath::FRandRange(400.0f, 500.0f);
	ProjectileMovement->MaxSpeed = 8000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Velocity = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(FVector::UpVector, 35.0f);

	// Class is set in Blueprint
	DamageWidget = PCIP.CreateDefaultSubobject<UDamageWidgetComponent>(this, TEXT("DamageWidget"));
	DamageWidget->SetWidgetSpace(EWidgetSpace::Screen);
	DamageWidget->SetDrawSize({ 100.f, 100.0f });
	DamageWidget->SetDrawAtDesiredSize(true);
	DamageWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADamageIndicator::BeginPlay()
{
	Super::BeginPlay();

	//Die after X seconds.
	SetLifeSpan(DamageIndicatorLifeSpan);
}
