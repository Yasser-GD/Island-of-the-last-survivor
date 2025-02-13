// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player_Interface.h"
#include "Inventory_System_AC.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Survivor_Character.generated.h"


class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUseItem_SC, ASurvivor_Character*, CharacterRef);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleaseItem_SC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingSystemON_OFF_SC, bool, IsBuildingSystemON);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayer_TakingDamage_SC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealth_Update_SC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHungerChange_SC);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemovingSomething_Pressed_SC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemovingSomething_Released_SC);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeath_SC);


UENUM(BlueprintType)
enum class EMovementType_SC : uint8
{
	NotHoldingAnyThing,
	HoldingSecondaryItem,
	CarryingBuildItem,
	HoldingWeapon,

};

UENUM(BlueprintType)
enum class EWeaponType_SC : uint8
{

	Axe = 0,
	Sword = 1,
	Machete = 2,

};


UCLASS()
class TESTINGMOVEMENNTS_API ASurvivor_Character : public ACharacter, public IPlayer_Interface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASurvivor_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnPlayerTakeDamage(float DamageTaken) override;

public:

	/** First person camera */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMovementType_SC MovementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EMovementType_SC LastMovementState_HeWasIn;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EWeaponType_SC WeaponType;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable)
		FOnUseItem_SC OnUseItem;

	UPROPERTY(BlueprintAssignable)
		FOnReleaseItem_SC OnReleaseItem;

	UPROPERTY(BlueprintAssignable)
		FPlayerDeath_SC OnPlayerDeath;

	//On Player Taking Damage Delegete
	UPROPERTY(BlueprintAssignable)
		FOnPlayer_TakingDamage_SC OnTakingDamage_Del;

	UPROPERTY(BlueprintAssignable)
		FOnHealth_Update_SC OnHealth_Update_Del;

	//On Hunger Change Delegete
	UPROPERTY(BlueprintAssignable)
		FOnHungerChange_SC OnHungerChange_Del;


	UPROPERTY(BlueprintAssignable)
		FOnRemovingSomething_Pressed_SC OnRemovingSomething_Pressed_Del;
	UPROPERTY(BlueprintAssignable)
		FOnRemovingSomething_Released_SC OnRemovingSomething_Released_Del;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable)
		FOnBuildingSystemON_OFF_SC OnBuildingSystemON_OFF_Del;

	class UCPP_Game_Ins_tance* GameInstanceRef;

public:

	//-----------------------------------------------------------------
	//----------------- PLAYER SLOTS ----------------------
	//-----------------------------------------------------------------

	UPROPERTY(BlueprintReadwrite)
		AItem_Base_Actor* Slot_1A;
	UPROPERTY(BlueprintReadwrite)
		bool alreadyEquiped_Slot_1 = false;

	UPROPERTY(BlueprintReadwrite)
		AItem_Base_Actor* Slot_2A;
	UPROPERTY(BlueprintReadwrite)
		bool alreadyEquiped_Slot_2 = false;

	UPROPERTY(BlueprintReadwrite)
		AItem_Base_Actor* Slot_3A;
	UPROPERTY(BlueprintReadwrite)
		bool alreadyEquiped_Slot_3 = false;

	UPROPERTY(BlueprintReadwrite)
		AItem_Base_Actor* Slot_4A;
	UPROPERTY(BlueprintReadwrite)
		bool alreadyEquiped_Slot_4 = false;

	UFUNCTION()
		void EquipItem_Slot_1();

	UFUNCTION()
		void EquipItem_Slot_2();

	UFUNCTION()
		void EquipItem_Slot_3();

	UFUNCTION()
		void EquipItem_Slot_4();

	UFUNCTION()
		void UnEquipItems_ToSlots();



	//-----------------------------------------------------------------
	//----------------- PLAYER SLOTS End ----------------------
	//-----------------------------------------------------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool HasDiary = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool HasShipBlueprint = false;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		bool HasLantern = false;


public:

	/** Fires a projectile. */
	void OnPrimaryAction();
	/** Fires a projectile. */
	void OnReleaseAction();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;


public:

	UFUNCTION()
		void ToLineToTryGetHeight();

	UFUNCTION()
		void Running_Pressed();
	UFUNCTION()
		void Running_Released();

	UFUNCTION()
		void Dash();

	FTimerHandle SpeedingTime;
	FTimerHandle SlowingDownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Basic Values")
		float WalkSpeed = 150;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Basic Values")
		float RunSpeed = 600;


	//-----------------------------------------------------------------
	//----------------- Hunger Functionilty ----------------------
	//-----------------------------------------------------------------

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Basic Values")
		float Hunger = 50.0;

	UFUNCTION()
		void HungerDecreasing();

	UFUNCTION()
		void HealthRestoration_FromHunger();
	FTimerHandle HealthRestorationTimer;


public:

	UFUNCTION()
		void Interact_PressE();

	FVector BaseCameraLoc;


public:

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Basic Values")
		float PlayerHealth = 200;
	UPROPERTY(BlueprintReadwrite)
		float InitialHealth;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Basic Values")
		bool IsPlayerDead = false;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UPhysicsHandleComponent* PhysicsHandleCompObject;


	//------------------Building System Properties--------------------------


	UFUNCTION()
		void BuildingSystem_ON();
	UFUNCTION()
		void BuildingSystem_OFF();
	UFUNCTION()
		void IsCarring_UnStorableItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building System")
		TSubclassOf<AActor> BuildingMaster_Actor;
	UPROPERTY(BlueprintReadWrite)
		AActor* SpawnedActor;

	bool AlreadySpawnedBuildingMaster = false;


	bool IsBuilding_ON = false;


	UFUNCTION()
		void OnRemovingSomething_Pressed();

	UFUNCTION()
		void OnRemovingSomething_Releasing();

	//----------------------Building System--------------------END------------


public:

	//------------------------ Inventory System -----------------------------
	UFUNCTION()
		UInventory_System_AC* GetInventory_AC();

	UPROPERTY(BlueprintReadWrite)
		int32 AttackCount_Player = 0;
	UPROPERTY(BlueprintReadWrite)
		bool CanMake_AnotherAttack = false;





};
