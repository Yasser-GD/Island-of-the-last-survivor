// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item_Base_Actor.h"
#include "IBA_Log.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickingUp_UnStorableItem);

UCLASS()
class TESTINGMOVEMENNTS_API AIBA_Log : public AItem_Base_Actor
{
	GENERATED_BODY()


public:

	//virtual void BeginPlay() override;

	virtual void Interactpure(ASurvivor_Character* CharacterRef) override;

	int32 StoringIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FName> Sockets_ToCarry_UnStoreableItems;

	UPROPERTY(BlueprintAssignable)
		FOnPickingUp_UnStorableItem OnPickingUp_UnstorableItem_Del;

	bool alreadyBroadcasted = false;

};
