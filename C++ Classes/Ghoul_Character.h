// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Damage_System_Interface.h"

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ghoul_Character.generated.h"


class ATestingMovemenntsCharacter;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBoxComponent;
class USoundCue;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTakingDamage, FName, BoneName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlaySound);



UENUM(BlueprintType)
enum class EGhoulMovementState : uint8 {

	Crawling_Movement = 0,
	Normal_Movement = 1,
	Death_State = 2,

};


UCLASS()
class TESTINGMOVEMENNTS_API AGhoul_Character : public ACharacter, public IDamage_System_Interface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGhoul_Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void TakeDamageAllEnemys(float DamageAmount, FName BoneName) override;
	bool CanBeDamaged = true;

public:

	//-------------------------- Delegetes ----------------------------
	UPROPERTY(BlueprintAssignable)
		FOnTakingDamage OnTakingDamage;

	UPROPERTY(BlueprintAssignable)
		FOnDeath OnDeath_Del;

	UPROPERTY(BlueprintAssignable)
		FOnPlaySound OnplaySound_Del;

	UFUNCTION()
		void AfterDeath();

public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		EGhoulMovementState GhoulMovementState;

	UPROPERTY(BlueprintReadWrite)
		ASurvivor_Character* PlayerRef;

private:

	APawn* Playerpawn;

	ASurvivor_Character* PlayerSensingRef;

	class AGhoul_AIController* Ghoul_AIConRef;

public:


	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		bool Ghoul_Dead = false;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		bool IsAlreadyDead_G = false;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		float Ghoul_Health = 500;
	float initial_Health_G;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "The basics")
		float GhoulDamage = 30.0;

	UFUNCTION()
		void StartWithPlayerSensed();

	UFUNCTION()
		void CheckIfGhoulStoppedMoving_StartWithPlayer();


	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Developer Stuff")
		bool MoveToPlayerOnSpawn = false;


private:
	//---------------------------Move Around Functionlity -------------------------




	//--------------------------------- Sensing Functionilty ----------------------------------------

	UPROPERTY(meta = (AllowPrivateAccess = "true"))
		UAIPerceptionComponent* AIPercCom;
	UPROPERTY()
		UAISenseConfig_Sight* SightSense;

	UFUNCTION()
		void OnSensed(const TArray<AActor*>& UpdatedActors);

	UPROPERTY()
		bool PlayerSensed = false;

	//--------------------------------- END ----------------------------------------

	UFUNCTION()
		void RotateTowardTarget(FVector TargetLocation, FVector BaseLocation);

	UFUNCTION()
		void RotateToPlayerOnSensed();

	UFUNCTION()
		void ChangeSpeed(bool IsWalking);

public:

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ghoul Speed")
		float WalkSpeed = 130;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Ghoul Speed")
		float RunSpeed = 700;

private:

	//-------------------------------- Attacking Functionlity ------------------------------------

	UPROPERTY(EditAnyWhere, Category = "Attack Montages")
		UAnimMontage* AttackOne;
	UPROPERTY(EditAnyWhere, Category = "Attack Montages")
		UAnimMontage* AttackTwo;
	UPROPERTY(EditAnyWhere, Category = "Attack Montages")
		UAnimMontage* AttackThree;

	UFUNCTION()
		void FTakingHitReaction();

	UPROPERTY(EditAnyWhere, Category = "Hit Reaction Montages")
		UAnimMontage* TakingHit_Reaction_1;
	UPROPERTY(EditAnyWhere, Category = "Hit Reaction Montages")
		UAnimMontage* TakingHit_Reaction_2;
	UPROPERTY(EditAnyWhere, Category = "Hit Reaction Montages")
		UAnimMontage* TakingHit_Reaction_3;

	int32 HitReactionNumber = 0;


	//Function To Set Timer To Call The Damaging Function Muiltible Times 
	UFUNCTION(BlueprintCallable)
		void ToCallDamagingFunction();

	//The Damaging Timer 
	FTimerHandle DamagingTimer;

public:

	UPROPERTY(BlueprintReadWrite)
		bool IsDamaging = false;
	UPROPERTY(BlueprintReadWrite)
		bool DamagedOnce = false;

private:

	//To Make A Sweep Hit Sphere To Damage The Player When Attacking 
	UFUNCTION(BlueprintCallable)
		void MakingDamagingSphere();
	UPROPERTY(EditAnyWhere)
		float SphereRadius = 30.0;

	UPROPERTY(EditAnyWhere, Category = "Hitting Player Sound")
		USoundBase* DamagingPlayerSound;


	UPROPERTY(EditAnyWhere, Category = "Attack Montages")
		UAnimMontage* ScreamMontage;

	UPROPERTY(EditAnyWhere, Category = "Death Anim Montage")
		UAnimMontage* Death_Montage;

	UFUNCTION(BlueprintCallable)
		void StartAttackingFunctionlity();

	

	UFUNCTION(BlueprintCallable)
		void CallAttack_InAICon();

	int32 AttackNumber = 0;

public:

	UPROPERTY(BlueprintReadWrite)
		bool OverlappedAttackBox = false;



	UFUNCTION(BlueprintCallable)
		void ToClearAllTimers_IfPlayerRestart();



};
