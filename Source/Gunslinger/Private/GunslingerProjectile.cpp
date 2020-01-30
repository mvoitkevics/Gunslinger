// Fill out your copyright notice in the Description page of Project Settings.


#include "GunslingerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGunslingerProjectile::AGunslingerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));
}

// Called when the game starts or when spawned
void AGunslingerProjectile::BeginPlay()
{
	Super::BeginPlay();
}
