// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Gunslinger.h"
#include "Engine/AssetManager.h"
#include "GunslingerAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UGunslingerAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	UGunslingerAssetManager() {}


	virtual void StartInitialLoading() override;

	/** Static types for items */
	static const FPrimaryAssetType	UtilityItemType;
	static const FPrimaryAssetType	ArmorItemType;
	static const FPrimaryAssetType	TokenItemType;
	static const FPrimaryAssetType	WeaponItemType;
	static const FPrimaryAssetType	AmmoItemType;

	/** Returns the current AssetManager object */
	static UGunslingerAssetManager& Get();

	/**
	 * Synchronously loads an RPGItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	class UGunslingerItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};
