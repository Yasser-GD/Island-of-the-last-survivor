// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemsData.h"
#include "InteractInter_face.h"
#include "Weapons_Interface.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Base_Actor.generated.h"


class ATestingMovemenntsCharacter;

UCLASS()
class TESTINGMOVEMENNTS_API AItem_Base_Actor : public AActor, public IInteractInter_face, public IWeapons_Interface
{
	GENERATED_BODY()

public:

	AItem_Base_Actor();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	virtual void Interactpure(ASurvivor_Character* CharacterRef) override;
	virtual void Interact_Imp_Implementation(ASurvivor_Character* CharacterRef) override;


	// if the item is a weapon he will have his own functionalty of those functions 
	virtual	void Equip_Weapon(ASurvivor_Character* CharacterRef) override;
	virtual	void UnEquip_Weapon(ASurvivor_Character* CharacterRef) override;
	virtual	void Use_Weapon(ASurvivor_Character* CharacterRef) override;


	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		UStaticMeshComponent* ItemMesh;

	class ASurvivor_Character* CharacterRefCasting;

public:

	//------------------------------- Items properties ------------------------------------
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		FName DesiredID;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		int Quantity;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		EItemStoringType ItemStoringType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		EItemType ItemType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		FItemStatices ItemStatices;

	UPROPERTY(EditAnyWhere, Category = "Item Data")
		FItemAssetData ItemAssetData;

	UPROPERTY(EditAnyWhere, Category = "Item Data")
		UDataTable* ItemDataTable;

	// On Spawn the item it gets its information from the data table (Mesh, name, item type , etc)
	void InitializePickUp();

	UPROPERTY(BlueprintReadWrite, Category = "Item Data")
		AItem_Base_Actor* NewItemReferance;

	AItem_Base_Actor* CreateNewItemCopy();


	UFUNCTION(BlueprintCallable)
		virtual	void Use(ASurvivor_Character* CharacterRef);

	UFUNCTION(BlueprintCallable)
		void DropItem(ASurvivor_Character* CharacterRef, TSubclassOf<AItem_Base_Actor> DroppedNewItem);

	UPROPERTY(EditAnyWhere, Category = "Dropping Item")
		TSubclassOf<AItem_Base_Actor> DroppedNewItemRef;

	UFUNCTION(BlueprintCallable)
		virtual	void AttackingFunctionilty();
	UPROPERTY(BlueprintReadWrite)
		bool AlreadyHit = false;

	UPROPERTY(BlueprintReadWrite, Category = "Attack Animations")
		bool IsStartingDamage = false;

};
