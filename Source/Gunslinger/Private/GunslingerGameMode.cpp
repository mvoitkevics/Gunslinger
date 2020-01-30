// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GunslingerGameMode.h"
#include "GunslingerPlayerController.h"
#include "CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AGunslingerGameMode::AGunslingerGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AGunslingerPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGunslingerGameMode::BeginPlay()
{
	OnCharacterDied.AddDynamic(this, &AGunslingerGameMode::RegisterCharacterDeath);
}

void AGunslingerGameMode::RegisterCharacterDeath(ACharacterBase* Character)
{
	// Drop shit

	// Tell pawn to re-scan(hack fo rnow)

	if (AGunslingerPlayerController* CharPC = Cast<AGunslingerPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		// change to BP_Scan() later;
		CharPC->bDidSearch = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Somebody died"));
}
