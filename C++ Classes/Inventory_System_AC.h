// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory_System_AC.generated.h"




class AItem_Base_Actor;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TESTINGMOVEMENNTS_API UInventory_System_AC : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventory_System_AC();

protected:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	AItem_Base_Actor* ItemTakenRef;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TArray<AItem_Base_Actor*> Item_StoringArray;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TArray<AItem_Base_Actor*> Items_DestoryingArray;

};
