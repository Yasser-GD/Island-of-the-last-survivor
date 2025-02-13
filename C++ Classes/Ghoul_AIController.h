// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Ghoul_AIController.generated.h"


class AGhoul_Character;


UCLASS()
class TESTINGMOVEMENNTS_API AGhoul_AIController : public AAIController
{
	GENERATED_BODY()


public:


	AGhoul_Character* GhoulMainRef;

	UFUNCTION()
		void Attack(AGhoul_Character* GhoulRef, int32 AttackNumber, UAnimMontage* AttackOne, UAnimMontage* AttackTwo, UAnimMontage* AttackThree);

	UFUNCTION()
		void MoveAround(AGhoul_Character* GhoulRef);
	bool AlreadChosethisMove = false;

	//To See if he is moving to point 
	bool MovingToAPoint = false;

	//To See If He Is Attacking 
	bool IsAttacking = false;


	UFUNCTION()
		void MoveAroundWhileAttacking(AGhoul_Character* GhoulRef);

	//------------------Sounds--------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sounds")
		class USoundBase* GhoulMovingAroundSound;

};
