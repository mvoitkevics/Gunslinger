// Fill out your copyright notice in the Description page of Project Settings.


#include "GunslingerAssetManager.h"
#include "Items/GunslingerItem.h"
#include "AbilitySystemGlobals.h"


const FPrimaryAssetType	UGunslingerAssetManager::UtilityItemType = TEXT("Utility");
const FPrimaryAssetType	UGunslingerAssetManager::ArmorItemType = TEXT("Armor");
const FPrimaryAssetType	UGunslingerAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UGunslingerAssetManager::WeaponItemType = TEXT("Weapon");
const FPrimaryAssetType	UGunslingerAssetManager::AmmoItemType = TEXT("Ammo");



UGunslingerAssetManager& UGunslingerAssetManager::Get()
{
	UGunslingerAssetManager* This = Cast<UGunslingerAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be RPGAssetManager!"));
		return *NewObject<UGunslingerAssetManager>(); // never calls this
	}
}

void UGunslingerAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}


UGunslingerItem* UGunslingerAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UGunslingerItem* LoadedItem = Cast<UGunslingerItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}