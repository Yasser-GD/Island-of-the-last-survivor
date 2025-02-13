// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damage_System_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamage_System_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class TESTINGMOVEMENNTS_API IDamage_System_Interface
{
	GENERATED_BODY()

		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void TakeDamageAllEnemys(float DamageAmount, FName BoneName) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		 void EnemysTakingDamage_Imp(float DamageAmount, FName BoneName);

	/*UPROPERTY(EditAnyWhere)
		bool AlreadyDamaged*/

};
