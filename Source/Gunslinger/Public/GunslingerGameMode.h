// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CharacterBase.h"
#include "GunslingerGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, ACharacterBase*, Character);

UCLASS()
class AGunslingerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGunslingerGameMode();

	virtual void BeginPlay() override;

	UFUNCTION()
	void RegisterCharacterDeath(ACharacterBase* Character);

	UPROPERTY(BlueprintAssignable, Category = "Character")
		FCharacterDiedDelegate OnCharacterDied;
};



