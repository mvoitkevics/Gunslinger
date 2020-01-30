// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EngineMinimal.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"


DECLARE_LOG_CATEGORY_EXTERN(LogGunslinger, Log, All);


UENUM(BlueprintType)
enum class EStates : uint8
{
	Stopped					UMETA(DisplayName = "Stopped"),
	Moving					UMETA(DisplayName = "Moving"),
	Attacking				UMETA(DisplayName = "Attacking"),
	Scanning				UMETA(DisplayName = "Scanning"),
};

UENUM(BlueprintType)
enum class EGameWeaponType : uint8
{
	None					UMETA(DisplayName = "Nothing is equipped"),
	Pistol					UMETA(DisplayName = "Pistol Weapon"),
	Rifle					UMETA(DisplayName = "Rifle Weapon"),
	Shotgun					UMETA(DisplayName = "Shotgun Weapon"),
	Launcher				UMETA(DisplayName = "Launcher Weapon"),
	Laser					UMETA(DisplayName = "Laser Weapon"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Single				UMETA(DisplayName = "Single Shot"),
	Spread				UMETA(DisplayName = "Spread"),
	Projectile			UMETA(DisplayName = "Projectile")
};

UENUM(BlueprintType)
enum class EItemTiers : uint8
{
	Old						UMETA(DisplayName = "Old item"),
	Normal					UMETA(DisplayName = "Normal Item"),
	Refined					UMETA(DisplayName = "Refined Item"),
	Rare					UMETA(DisplayName = "Rare Item"),
	Epic					UMETA(DisplayName = "Epic Item"),
	Legendary				UMETA(DisplayName = "Legendary Item"),
	Mythic					UMETA(DisplayName = "Mythic Item"),
};


UENUM(BlueprintType)
enum class EItemType : uint8
{
	// Armors
	Armor				UMETA(DisplayName = "Armor Item"),

	// Weapons
	Weapon				UMETA(DisplayName = "Weapon Item"),

	// Gold, Scrolls
	Token				UMETA(DisplayName = "Token"),

	//Ammo
	Ammo				UMETA(DisplayName = "Ammo"),

	// Utility items
	Utility				UMETA(DisplayName = "Consumable")
};


UENUM(BlueprintType)
enum class EAbilityUpgradeType : uint8
{
	// Armors
	Weapon				UMETA(DisplayName = "Weapon Upgrade, Changes Shooting"),

	// Weapons
	Projectile			UMETA(DisplayName = "On Hit Upgrades"),

	// Gold, Scrolls
	Character			UMETA(DisplayName = "Player Stats Upgrade"),

	Pet					UMETA(DisplayName = "TODO"),
};