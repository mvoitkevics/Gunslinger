// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "GunslingerPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GunslingerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "UI/HUDWidget.h"
#include "Player/GunslingerPlayerState.h"

AGunslingerPlayerController::AGunslingerPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AGunslingerPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
	{
		return;
	}

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), TEXT(__FUNCTION__));
		return;
	}

	// Need a valid PlayerState to get attributes from
	AGunslingerPlayerState* PS = GetPlayerState<AGunslingerPlayerState>();
	if (!PS)
	{
		return;
	}

	UIHUDWidget = CreateWidget<UHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	// Set attributes
	/*UIHUDWidget->SetCurrentHealth(PS->GetHealth());
	UIHUDWidget->SetMaxHealth(PS->GetMaxHealth());
	UIHUDWidget->SetHealthPercentage(PS->GetHealth() / PS->GetMaxHealth());*/
	/*UIHUDWidget->SetCurrentMana(PS->GetMana());
	UIHUDWidget->SetMaxMana(PS->GetMaxMana());
	UIHUDWidget->SetManaPercentage(PS->GetMana() / PS->GetMaxMana());
	UIHUDWidget->SetHealthRegenRate(PS->GetHealthRegenRate());
	UIHUDWidget->SetManaRegenRate(PS->GetManaRegenRate());
	UIHUDWidget->SetCurrentStamina(PS->GetStamina());
	UIHUDWidget->SetMaxStamina(PS->GetMaxStamina());
	UIHUDWidget->SetStaminaPercentage(PS->GetStamina() / PS->GetMaxStamina());
	UIHUDWidget->SetStaminaRegenRate(PS->GetStaminaRegenRate());
	UIHUDWidget->SetExperience(PS->GetXP());
	UIHUDWidget->SetGold(PS->GetGold());
	UIHUDWidget->SetHeroLevel(PS->GetCharacterLevel());*/

	UIHUDWidget->SetLevel(PS->GetCharacterLevel());
	UIHUDWidget->SetExpPercentage(PS->GetXP() / 1000);
}

UHUDWidget* AGunslingerPlayerController::GetHUD()
{
	return UIHUDWidget;
}

void AGunslingerPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(CurrentDestination, MyPawn->GetActorLocation());

		if (Distance <= 120.0f && !bDidSearch)
		{
			// Can scan for enemies

			if (ACharacterBase* AsBaseCharacter = Cast<ACharacterBase>(MyPawn))
			{
				// Tell Blueprint we ended movement
				AsBaseCharacter->BP_MoveEnd();

				// Tell Blueprints to make a scan for enemies
				AsBaseCharacter->BP_Scan();

				if (AsBaseCharacter->CurrentTarget)
				{
					// Dont render prev target
					AsBaseCharacter->CurrentTarget->GetMesh()->SetRenderCustomDepth(false);
				}

				// if new target found
				if (FindNewTarget(MyPawn))
				{
					if (AsBaseCharacter->CurrentTarget)
					{
						// switch to new target
						AsBaseCharacter->CurrentTarget->GetMesh()->SetRenderCustomDepth(true);

						FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MyPawn->GetActorLocation(), AsBaseCharacter->CurrentTarget->GetActorLocation());

						SetControlRotation(TargetRotation);

						AsBaseCharacter->CurrentState = EStates::Attacking;
						AsBaseCharacter->BP_PerformAttack();
					}
				}
				else {
					// stop shooting, since none to shoot
					AsBaseCharacter->BP_EndAttack();
				}
			}

			bDidSearch = true;
		}
	}
}

bool AGunslingerPlayerController::FindNewTarget(APawn* MyPawn)
{

	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// start and end locations
	FVector SweepStart = MyPawn->GetActorLocation();
	FVector SweepEnd = MyPawn->GetActorLocation() + FVector(0.1, 0, 0);

	ACharacterBase* AsBaseCharacter = Cast<ACharacterBase>(MyPawn);
	float ClosestDistance = 700.0f;

	if (AsBaseCharacter)
	{
		ClosestDistance = AsBaseCharacter->GetRange();
	}
	
	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(ClosestDistance);

	// draw collision sphere
	// DrawDebugSphere(GetWorld(), MyPawn->GetActorLocation(), MyColSphere.GetSphereRadius(), 10, FColor::Purple, true);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, SweepStart, SweepEnd, FQuat::Identity, ECC_WorldDynamic, MyColSphere);
	bool bFound = false;
	if (isHit)
		{
			ACharacterBase* ClosestCharacter = nullptr;

			// loop through TArray
			for (auto& Hit : OutHits)
			{
				if (GEngine)
				{
					// ignore selves
					if (Hit.Actor == MyPawn) continue;

					// uncommnet to see more info on sweeped actor
					// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("All Hit Information: %s"), *Hit.ToString()));

					// find closest
					if (Hit.Distance <= ClosestDistance) {
						ClosestDistance = Hit.Distance;

						if (ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Hit.Actor))
						{
							if (TargetCharacter != ClosestCharacter)
							{
								// screen log information on what was hit
								ClosestCharacter = TargetCharacter;
								bFound = true;
							}

						}
					}
				}
			}
			if (AsBaseCharacter)
			{
				AsBaseCharacter->CurrentTarget = ClosestCharacter;
			}

			return bFound;
		}
	
	return bFound;
}

void AGunslingerPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AGunslingerPlayerState* PS = GetPlayerState<AGunslingerPlayerState>();
	if (PS)
	{
		// Init ASC with PS (Owner) and our new Pawn (AvatarActor)
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	}
}

void AGunslingerPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}

void AGunslingerPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AGunslingerPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AGunslingerPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AGunslingerPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AGunslingerPlayerController::MoveToTouchLocation);

}


void AGunslingerPlayerController::MoveToMouseCursor()
{

	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		CurrentDestination = SetNewMoveDestination(Hit.ImpactPoint, GetPawn());
	}

}

void AGunslingerPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint, GetPawn());

		if (Role < ROLE_Authority)
		{
			Server_SetNewMoveDestination(HitResult.ImpactPoint, GetPawn());
		}
	}
}

FVector AGunslingerPlayerController::SetNewMoveDestination(const FVector DestLocation, APawn* TargetPawn)
{

	// APawn* const MyPawn = GetPawn();
	if (TargetPawn)
	{
		float const Distance = FVector::Dist(DestLocation, TargetPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(TargetPawn->GetActorLocation(), DestLocation);

			SetControlRotation(TargetRotation);

			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);

			if (ACharacterBase* AsBaseCharacter = Cast<ACharacterBase>(TargetPawn))
			{
				AsBaseCharacter->CurrentState = EStates::Moving;
				AsBaseCharacter->BP_MoveStart(DestLocation);
			}
			
			return DestLocation;
		}
		else {
			return TargetPawn->GetActorLocation();
		}
	}

	return { 0.f,0.f,0.f };
}

void AGunslingerPlayerController::Server_SetNewMoveDestination_Implementation(const FVector DestLocation, APawn* TargetPawn)
{
	SetNewMoveDestination(DestLocation, TargetPawn);
}

bool AGunslingerPlayerController::Server_SetNewMoveDestination_Validate(const FVector DestLocation, APawn* TargetPawn)
{
	return true;
}

void AGunslingerPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
	bDidSearch = false;
}

void AGunslingerPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
