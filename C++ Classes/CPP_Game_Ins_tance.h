// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CPP_Game_Ins_tance.generated.h"

UENUM(BlueprintType)
enum class EWaves : uint8 {
	NoWave = 0,
	Wave1 = 1,
	Wave2 = 2,
	Wave3 = 3

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDayChange , int32 , DayNumber );

UCLASS()
class TESTINGMOVEMENNTS_API UCPP_Game_Ins_tance : public UGameInstance
{
	GENERATED_BODY()

public:

	enum WavesState { W1, W2, W3 };
	WavesState Wave;

	UPROPERTY(BlueprintReadwrite)
		bool Wave1 = false;
	UPROPERTY(BlueprintReadwrite)
		bool Wave2 = false;
	UPROPERTY(BlueprintReadwrite)
		bool Wave3 = false;

	bool EnemyWaveAreDead = false;

	bool AbleToStartWave = true;

	UPROPERTY()
		float CountEnemyDead = 2;

	UPROPERTY(BlueprintReadwrite)
		bool IsPuzzleModeOn = false;

public:

	UPROPERTY(BlueprintReadwrite)
		bool bShowCursor = false;

	//Choices Test 
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool Choice_ToTakeHer = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool Choice_ToGiveHerSyraine = false;

public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsBuildingSystem_ON = false;


	
	//------------------- Day And Night Properties ------------------ this is the only things that are used in the game -------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool Night = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool Day = false;

	UPROPERTY(BlueprintAssignable, BlueprintCallable )
		FOnDayChange OnDayNightChange;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float DayRotation;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 DayNumber = 0;


	//--------------------Leaving The Island-------------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool IsLeaving = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TArray<FText> Instructions_texts;
	

};

