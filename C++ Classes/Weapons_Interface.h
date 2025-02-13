// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Weapons_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeapons_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TESTINGMOVEMENNTS_API IWeapons_Interface
{
	GENERATED_BODY()


public:

	UFUNCTION()
		virtual	void Equip_Weapon(ASurvivor_Character* CharacterRef) = 0;

	UFUNCTION()
		virtual	void UnEquip_Weapon(ASurvivor_Character* CharacterRef) = 0;

	UFUNCTION()
		virtual	void Use_Weapon(ASurvivor_Character* CharacterRef) = 0;

};
