// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemsData.generated.h"

UENUM()
enum class EItemStoringType : uint8
{

	Storable = 0,
	CantBeStored = 1,

};

UENUM()
enum class EItemType : uint8
{

	Weapon UMETA(DisplayName = "Weapon"),
	Shield UMETA(DisplayName = "Shield"),
	Armor UMETA(DisplayName = "Armor"),
	Consumable UMETA(DisplayName = "Consumable"),
	Build UMETA(DisplayName = "Build"),

};

USTRUCT(BlueprintType)
struct FItemStatices
{

	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		int MaxQuantity;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		float ArmorRating;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		float DamageAmount;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
		float RestorationAmount;



};


class AItem_Base_Actor;

USTRUCT(BlueprintType)
struct FItemAssetData
{

	GENERATED_USTRUCT_BODY()


public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		class UMaterial* MeshMaterial;

	UPROPERTY(EditAnyWhere , BlueprintReadWrite)
		class UStaticMesh* ItemMesh_ItemData;

	UPROPERTY(EditAnyWhere)
		FVector DesiredScale = FVector(1.0, 1.0, 1.0);

	UPROPERTY(EditAnyWhere)
		TSubclassOf<AItem_Base_Actor> NewItem_ToDrop;

};


USTRUCT()
struct FItemData : public FTableRowBase
{

	GENERATED_USTRUCT_BODY()


public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		FName ID;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		EItemStoringType ItemStoringType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Item Data")
		EItemType ItemType;

	UPROPERTY(EditAnyWhere, Category = "Item Data")
		FItemStatices ItemStatices;

	UPROPERTY(EditAnyWhere, Category = "Item Data")
		FItemAssetData ItemAssetData;


};


