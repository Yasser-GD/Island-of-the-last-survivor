// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghoul_Character.h"

#include "TestingMovemenntsCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Sound\SoundCue.h"
#include "Ghoul_AIController.h"
#include "Player_Interface.h"
#include "Survivor_Character.h"



// Sets default values
AGhoul_Character::AGhoul_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIPercCom = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	SightSense = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Sense"));

	AIPercCom->ConfigureSense(*SightSense);
	AIPercCom->SetDominantSense(SightSense->GetSenseImplementation());

	SightSense->PeripheralVisionAngleDegrees = 90.0;
	SightSense->SightRadius = 2500.0;
	SightSense->LoseSightRadius = 3000;
	SightSense->DetectionByAffiliation.bDetectEnemies = true;
	SightSense->DetectionByAffiliation.bDetectFriendlies = true;
	SightSense->DetectionByAffiliation.bDetectNeutrals = true;


}

// Called when the game starts or when spawned
void AGhoul_Character::BeginPlay()
{

	Super::BeginPlay();

	SpawnDefaultController();

	Playerpawn = UGameplayStatics::GetPlayerPawn(this, 0);

	PlayerRef = Cast<ASurvivor_Character>(UGameplayStatics::GetPlayerPawn(this, 0));

	Ghoul_AIConRef = Cast<AGhoul_AIController>(GetController());

	if (PlayerRef) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("The Player Refereance Is Valid ")));
	}

	AIPercCom->OnPerceptionUpdated.AddDynamic(this, &AGhoul_Character::OnSensed);


	// To Set A Timer To Keep Moving Around (Calling To The AI Controller)
	if (Ghoul_AIConRef) {

		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("The Ghoul AI Controller Refereance Is Valid ")));

		FTimerDelegate MoveAround_Del;
		MoveAround_Del.BindUFunction(Ghoul_AIConRef, "MoveAround", this);

		FTimerHandle MoveAround_Timer;
		GetWorldTimerManager().SetTimer(MoveAround_Timer, MoveAround_Del, 5.0, true);

	}


	FTimerHandle TimeToCheckIfSpawnWithPlayerSensed;
	GetWorldTimerManager().SetTimer(TimeToCheckIfSpawnWithPlayerSensed, this, &AGhoul_Character::StartWithPlayerSensed, 1.0, false);

	FTimerHandle TimeToCheckIfStoppedChasingPlayer;
	GetWorldTimerManager().SetTimer(TimeToCheckIfStoppedChasingPlayer, this, &AGhoul_Character::CheckIfGhoulStoppedMoving_StartWithPlayer, 10.0, true);


	FTimerHandle TimerToRotateTowardPlayer;
	GetWorldTimerManager().SetTimer(TimerToRotateTowardPlayer, this, &AGhoul_Character::RotateToPlayerOnSensed, 0.2, true);




}

// Called every frame
void AGhoul_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

// Called to bind functionality to input
void AGhoul_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGhoul_Character::TakeDamageAllEnemys(float DamageAmount, FName BoneName)
{

	if (!Ghoul_Dead) {

		Ghoul_Health -= DamageAmount;

		if (!PlayerSensed) {
			PlayerSensed = true;
			StartAttackingFunctionlity();
			ChangeSpeed(false);
		}

		//To BroadCast To The Blueprint To Say That He Took Damage
		OnTakingDamage.Broadcast(BoneName);

		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Hit Bone Name : %s "), *BoneName.ToString()));

		FTakingHitReaction();

		if (Ghoul_Health <= 0.0) {

			Ghoul_Dead = true;

			GhoulMovementState = EGhoulMovementState::Death_State;

			OnDeath_Del.Broadcast();

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			//PrimaryActorTick.bCanEverTick = true;

			FTimerHandle TimerAfterDeath;

			GetWorldTimerManager().SetTimer(TimerAfterDeath, this, &AGhoul_Character::AfterDeath, 120, false);

			if (Ghoul_AIConRef) {

				Ghoul_AIConRef->StopMovement();
				PlayerSensed = false;

				if (Death_Montage) {
					PlayAnimMontage(Death_Montage);
				}

			}

		}

	}

}


void AGhoul_Character::AfterDeath()
{

	Destroy();

}


void AGhoul_Character::StartWithPlayerSensed()
{

	if (MoveToPlayerOnSpawn) {

		ChangeSpeed(false);
		PlayerSensed = true;
		//Ghoul_AIConRef->IsAttacking = true;
		StartAttackingFunctionlity();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("Should Move To Player And Attack him ")));

	}
	else if (!MoveToPlayerOnSpawn) {
		ChangeSpeed(true);
	}



}

void AGhoul_Character::CheckIfGhoulStoppedMoving_StartWithPlayer()
{

	if (MoveToPlayerOnSpawn && GetVelocity() == FVector::ZeroVector) {

		ChangeSpeed(false);
		PlayerSensed = true;
		//Ghoul_AIConRef->IsAttacking = true;
		StartAttackingFunctionlity();
		//GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("Should Move To Player And Attack him ")));

	}

}


void AGhoul_Character::OnSensed(const TArray<AActor*>& UpdatedActors)
{

	if (!Ghoul_Dead) {

		for (AActor* SensedActor : UpdatedActors) {

			PlayerSensingRef = Cast<ASurvivor_Character>(SensedActor);

		}

		if (PlayerSensingRef) {

			for (int i = 0; i < UpdatedActors.Num(); i++) {

				FActorPerceptionBlueprintInfo ActorInfo;
				AIPercCom->GetActorsPerception(UpdatedActors[i], ActorInfo);

				if (ActorInfo.LastSensedStimuli[0].WasSuccessfullySensed()) {

					PlayerSensed = true;

					if (Ghoul_AIConRef && !Ghoul_AIConRef->IsAttacking) {

						ChangeSpeed(false);

						PlayAnimMontage(ScreamMontage);

						FTimerHandle StartAttackTimer;
						GetWorldTimerManager().SetTimer(StartAttackTimer, this, &AGhoul_Character::StartAttackingFunctionlity, 2.0 , false );

						//StartAttackingFunctionlity();
					}

					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Ghoul Sensed the player")));

					if (Ghoul_AIConRef && FVector::Dist(GetActorLocation(), PlayerRef->GetActorLocation()) > 3000 && !MoveToPlayerOnSpawn) {

						ChangeSpeed(true);
						Ghoul_AIConRef->IsAttacking = false;
						PlayerSensed = false;
						Ghoul_AIConRef->StopMovement();
						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("He Set The player Sensed To FALSE ")));

					}

				}
				else {

					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("He Set The player Sensed To FALSE ")));
					if (Ghoul_AIConRef && FVector::Dist(GetActorLocation(), PlayerRef->GetActorLocation()) > 3000 && !MoveToPlayerOnSpawn) {

						ChangeSpeed(true);
						Ghoul_AIConRef->IsAttacking = false;
						PlayerSensed = false;
						Ghoul_AIConRef->StopMovement();
						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("He Set The player Sensed To FALSE ")));

					}
					//PlayerSensed = false;
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Alien Cant See the player")));

				}

			}

		}

	}


}

void AGhoul_Character::RotateTowardTarget(FVector TargetLocation, FVector BaseLocation)
{

	if (PlayerSensed && GetVelocity() == FVector::ZeroVector) {

		FVector Distance = TargetLocation - BaseLocation;
		FRotator NewRotation = Distance.Rotation();
		NewRotation.Roll = 0.0;
		NewRotation.Pitch = 0.0;
		SetActorRotation(NewRotation);

		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Set Rotation Toward player : %s"), *NewRotation.ToString() ));

		if (FVector::Dist(GetActorLocation(), PlayerRef->GetActorLocation()) > 3000 && !MoveToPlayerOnSpawn) {

			ChangeSpeed(true);
			Ghoul_AIConRef->IsAttacking = false;
			PlayerSensed = false;
			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("He Set The player Sensed To FALSE ")));
			Ghoul_AIConRef->StopMovement();
		}

	}

}

void AGhoul_Character::RotateToPlayerOnSensed()
{

	if (!Ghoul_Dead) {

		if (PlayerSensed && GetVelocity() == FVector::ZeroVector ) {
			RotateTowardTarget(Playerpawn->GetActorLocation(), GetActorLocation());
		}

	}

}

void AGhoul_Character::ChangeSpeed(bool IsWalking)
{

	if (IsWalking) {

		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	}
	else {

		if (GhoulMovementState == EGhoulMovementState::Crawling_Movement) {
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		}
		else {
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		}

	}

}


void AGhoul_Character::FTakingHitReaction()
{

	int RandNum = FMath::RandRange(0, 2);

	if (TakingHit_Reaction_1 && RandNum == 0 && HitReactionNumber != RandNum) {

		PlayAnimMontage(TakingHit_Reaction_1);
		HitReactionNumber = RandNum;
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("First Anim ")));
	}
	else if (TakingHit_Reaction_2 && RandNum == 1 && HitReactionNumber != RandNum) {

		PlayAnimMontage(TakingHit_Reaction_2);
		HitReactionNumber = RandNum;
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("Second Anim ")));
	}
	else if (TakingHit_Reaction_3 && RandNum == 2 && HitReactionNumber != RandNum) {

		PlayAnimMontage(TakingHit_Reaction_3);
		HitReactionNumber = RandNum;
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Green, FString::Printf(TEXT("Third Anim ")));
	}
	else {
		FTakingHitReaction();
	}

}


void AGhoul_Character::ToCallDamagingFunction()
{

	if (!Ghoul_Dead) {
		GetWorldTimerManager().SetTimer(DamagingTimer, this, &AGhoul_Character::MakingDamagingSphere, 0.05, true);
	}

}

void AGhoul_Character::MakingDamagingSphere()
{


	if (!Ghoul_Dead && IsDamaging) {

		TArray<FHitResult> HitResults;
		FVector Start = GetMesh()->GetSocketLocation("Damaing Socket");
		FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);

		bool Hit = GetWorld()->SweepMultiByChannel(HitResults, Start, Start, FQuat::Identity, ECC_Camera, SphereShape);
		//DrawDebugSphere(GetWorld(), Start, SphereRadius, 2, FColor::Red, false, 0.2, 0.0, 3.0);

		if (Hit) {

			for (FHitResult OneHit : HitResults) {

				if (OneHit.GetActor()->GetClass()->ImplementsInterface(UPlayer_Interface::StaticClass()) && !DamagedOnce) {

					IPlayer_Interface* Player_InterfaceRef = Cast<IPlayer_Interface>(OneHit.GetActor());
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT(" Player has Interface ")));
					if (Player_InterfaceRef) {
						UGameplayStatics::PlaySoundAtLocation(this, DamagingPlayerSound, OneHit.Location);
						Player_InterfaceRef->OnPlayerTakeDamage(GhoulDamage);
						DamagedOnce = true;
						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT(" Hit Player ")));
					}

				}

			}

		}

		else {
			GetWorldTimerManager().ClearTimer(DamagingTimer);
		}
	}

}


void AGhoul_Character::StartAttackingFunctionlity()
{

	if (!Ghoul_Dead && PlayerRef) {

		if (!PlayerRef->IsPlayerDead) {


			int32 RandNum = FMath::RandRange(1, 3);
			float TimerRate = 5.0;
			bool IsMovingToPlayer = false;

			if (Ghoul_AIConRef) {

				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Blue, FString::Printf(TEXT("Started moving to a point before attacking ")));
				Ghoul_AIConRef->StopMovement();
				Ghoul_AIConRef->MoveAroundWhileAttacking(this);

				if (Ghoul_AIConRef->GhoulMovingAroundSound) {
					FTimerHandle PlaySoundTimer;
					GetWorldTimerManager().SetTimer(PlaySoundTimer, [&]() {OnplaySound_Del.Broadcast(); }, 1.0, false);

				}

				if (RandNum == 2) {

					FTimerHandle AttackTimer;
					GetWorldTimerManager().SetTimer(AttackTimer, [&]() {Ghoul_AIConRef->MoveAroundWhileAttacking(this); }, 3.0, false);
					TimerRate = 10.0;

				}

				Ghoul_AIConRef->IsAttacking = true;

			}

			FTimerHandle AttackTimer;
			GetWorldTimerManager().SetTimer(AttackTimer, [&]()
				{

					if (!Ghoul_Dead && PlayerRef) {

						if (!PlayerRef->IsPlayerDead) {

							Ghoul_AIConRef->MoveToActor(PlayerRef);
							IsMovingToPlayer = true;

							if (Ghoul_AIConRef->GhoulMovingAroundSound) {

								FTimerHandle PlaySoundTimer;
								GetWorldTimerManager().SetTimer(PlaySoundTimer, [&]() {OnplaySound_Del.Broadcast(); }, 1.0, false);

							}

						}

						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT(" Moving to the player  ")));
					}


				}, TimerRate, false);



			FTimerHandle MoveAgainToPlayerTimer;
			GetWorldTimerManager().SetTimer(MoveAgainToPlayerTimer, [&]()
				{

					if (!Ghoul_Dead && IsMovingToPlayer && GetVelocity() == FVector::ZeroVector && PlayerRef) {

						if (!PlayerRef->IsPlayerDead) {

							Ghoul_AIConRef->MoveToActor(PlayerRef);

							if (Ghoul_AIConRef->GhoulMovingAroundSound) {

								FTimerHandle PlaySoundTimer;
								GetWorldTimerManager().SetTimer(PlaySoundTimer, [&]() {OnplaySound_Del.Broadcast(); }, 1.0, false);

							}

						}

						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT(" Moving to the player  ")));
					}

				}, TimerRate + 4.0, false);


		}
	}

}


void AGhoul_Character::CallAttack_InAICon()
{

	if (!Ghoul_Dead) {

		if (Ghoul_AIConRef && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying()) {

			if (AttackOne && AttackNumber == 0) {

				PlayAnimMontage(AttackOne);

			}
			else if (AttackTwo && AttackNumber == 1) {

				PlayAnimMontage(AttackTwo);

			}
			else if (AttackThree && AttackNumber == 2) {

				PlayAnimMontage(AttackThree);

			}

			if (AttackNumber == 2) {
				AttackNumber = 0;
			}
			else {
				AttackNumber++;
			}


			FTimerHandle AttackTimer;
			GetWorldTimerManager().SetTimer(AttackTimer, [&]() {StartAttackingFunctionlity(); }, 2.0, false);

		}

	}



}

void AGhoul_Character::ToClearAllTimers_IfPlayerRestart()
{

	GetWorldTimerManager().ClearAllTimersForObject(this);

}
