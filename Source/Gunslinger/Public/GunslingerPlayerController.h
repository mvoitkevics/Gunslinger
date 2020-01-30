// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GunslingerPlayerController.generated.h"

class ACharacterBase;
class UHUDWidget;

UCLASS()
class AGunslingerPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGunslingerPlayerController();

	UPROPERTY(BlueprintReadOnly, Category = "AI")
		FVector CurrentDestination;

	void CreateHUD();

	class UHUDWidget* GetHUD();
	uint8 bDidSearch : 1;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	UFUNCTION(BlueprintCallable, Category = "Actions")
		bool FindNewTarget(APawn* MyPawn);

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	/** Navigate player to the current touch location. */
	void MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location);
	
	/** Navigate player to the given world location. */
	FVector SetNewMoveDestination(const FVector DestLocation, APawn* TargetPawn);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetNewMoveDestination(const FVector DestLocation, APawn* TargetPawn);
	void Server_SetNewMoveDestination_Implementation(const FVector DestLocation, APawn* TargetPawn);
	bool Server_SetNewMoveDestination_Validate(const FVector DestLocation, APawn* TargetPawn);

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();

	


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<class UHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		class UHUDWidget* UIHUDWidget;

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;
};


