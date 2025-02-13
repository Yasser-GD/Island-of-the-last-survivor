// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ItemsData.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Building_System_Base_AC.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildingSystemON);

UCLASS(BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TESTINGMOVEMENNTS_API UBuilding_System_Base_AC : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBuilding_System_Base_AC();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BindToInput();

	UPROPERTY(BlueprintAssignable)
		FOnBuildingSystemON OnBuildingSystemON_Del;

	UCPP_Game_Ins_tance* GameInstanceRef;

private:

	//class UInputComponent* InputComponent;
	UPROPERTY()
		AActor* Owner;
	UPROPERTY()
		APawn* PlayerPawn;

	class ASurvivor_Character* MainCharacter;

public:

	UFUNCTION()
		void BuildingSystem_ONOFF(bool IsBuildingSystemON);

	UPROPERTY(BlueprintReadwrite)
		bool IsBuildingModeON = false;

	UFUNCTION()
		void TraceToGet_AHitLocation();

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		AActor* HitActor;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		int32 NumToFlipFlop = 1;

private:

	//------------------- Removing A Building -------------

	UFUNCTION()
		void RemoveBuilding();

	UFUNCTION()
		void RemoveBuilding_Pressed();
	UFUNCTION()
		void RemoveBuilding_Released();

	bool IsRemovingBuilding = false;

	FTimerHandle RemovingBuildingTimer;

public:


	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		FText ID;
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool Is_ID_EqualsID = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UPrimitiveComponent* AlignmentComponentHit;
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		bool HitAnAlignmentComponent = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UStaticMeshComponent* MeshToPlace;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UStaticMesh* MainMesh;
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UStaticMesh* NoneMesh;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UMaterialInterface* MainMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
		UMaterialInterface* PreviewMaterial;

	UFUNCTION()
		void UnablePlaceBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void AblePlaceBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(BlueprintReadwrite)
		bool IsAbleToPlaceBuilding = true;


	UPROPERTY(EditAnyWhere, Category = "Item Data")
		UDataTable* UnStorableItems_DataTable;

};
