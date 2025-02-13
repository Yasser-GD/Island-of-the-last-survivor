// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Base_Actor.h"
#include "Axe_WeaponItem.generated.h"


UCLASS()
class TESTINGMOVEMENNTS_API AAxe_WeaponItem : public AItem_Base_Actor
{
	GENERATED_BODY()


protected:

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void Interactpure(ASurvivor_Character* CharacterRef) override;
	virtual void AttackingFunctionilty() override;


	virtual	void Equip_Weapon(ASurvivor_Character* CharacterRef) override;
	virtual	void UnEquip_Weapon(ASurvivor_Character* CharacterRef) override;
	virtual	void Use_Weapon(ASurvivor_Character* CharacterRef) override;

	int32 StoringIndex = 0;

public:

	//----------------Equip And UnEquip Socket Names---------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		FName EquipingWeaponSocketName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		FName UnEquipingWeaponSocketName;

	//-------------------Attack Animations-------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Attack Animations")
		UAnimMontage* AttackAnim_1;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Attack Animations")
		UAnimMontage* AttackAnim_2;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Attack Animations")
		UAnimMontage* AttackAnim_3;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Attack Animations")
		float PlayRate = 1.0;

	int AttacksCount = 0;


	//----------------Hiiting Functionality---------------

	UFUNCTION()
		void FToMakeSweepHitSphere();

	TArray<AActor*> ActorsGotHit;

	FTimerHandle HittingTimer;

	//-------------Attacking Helping Varibles------------------

	UPROPERTY(BlueprintReadWrite, Category = "Attacking")
		FName AttackingWeaponsSocketName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Damaging")
		float SphereRadious = 40;

	//---------------------------


	UFUNCTION()
		void OnDroppingItem(ASurvivor_Character* CharacterRef);


	//----------------------------- Sounds ----------------------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Sounds")
		USoundBase* HitSound;


};
