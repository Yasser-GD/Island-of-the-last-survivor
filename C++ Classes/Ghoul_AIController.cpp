// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghoul_AIController.h"

#include "Ghoul_Character.h"
#include "Kismet/GameplayStatics.h"


void AGhoul_AIController::MoveAround(AGhoul_Character* GhoulRef)
{


	if (!GhoulRef->Ghoul_Dead && !IsAttacking) {

		if (MovingToAPoint) {

			if (GhoulRef->GetVelocity() == FVector::ZeroVector) {
				MovingToAPoint = false;
			}

		}

		if (!MovingToAPoint) {

			int32 RandNum = FMath::RandRange(0, 3);
			float RandBigNumber = FMath::RandRange(100, 500);

			FVector NewLocation_Left = GhoulRef->GetActorLocation() + FVector(RandBigNumber, -800, 0.0);
			FVector NewLocation_Right = GhoulRef->GetActorLocation() + FVector(RandBigNumber, 800, 0.0);
			FVector NewLocation_Forward = GhoulRef->GetActorLocation() + FVector(-800, RandBigNumber, 0.0);
			FVector NewLocation_Backward = GhoulRef->GetActorLocation() + FVector(800, RandBigNumber, 0.0);

			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Blue, FString::Printf(TEXT("Moving Around  ")));

			switch (RandNum)
			{
			case(0):
				MoveToLocation(NewLocation_Forward);
				MovingToAPoint = true;
				break;
			case(1):
				MoveToLocation(NewLocation_Backward);
				MovingToAPoint = true;
				break;
			case(2):
				MoveToLocation(NewLocation_Right);
				MovingToAPoint = true;
				break;
			case(3):
				MoveToLocation(NewLocation_Left);
				MovingToAPoint = true;
				break;
			default:
				break;
			}

		}

	}

	/*if (GhoulMovingAroundSound) {

		FTimerHandle PlaySoundTimer;
		GetWorldTimerManager().SetTimer(PlaySoundTimer, [&]() {

			UGameplayStatics::PlaySoundAtLocation(GhoulRef, GhoulMovingAroundSound, GhoulRef->GetActorLocation());
			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT(" playing The Ghouls Sound ")));

			}, 2.0, false);

	}*/

}

void AGhoul_AIController::MoveAroundWhileAttacking(AGhoul_Character* GhoulRef)
{

	if (!GhoulRef->Ghoul_Dead) {

		if (MovingToAPoint) {

			if (GhoulRef->GetVelocity() == FVector::ZeroVector) {
				MovingToAPoint = false;
			}

		}


		if (!MovingToAPoint) {

			int32 RandNum = FMath::RandRange(0, 3);
			float RandBigNumber = FMath::RandRange(100, 500);

			FVector NewLocation_Left = GhoulRef->GetActorLocation() + FVector(RandBigNumber, -800, 0.0);
			FVector NewLocation_Right = GhoulRef->GetActorLocation() + FVector(RandBigNumber, 800, 0.0);
			FVector NewLocation_Forward = GhoulRef->GetActorLocation() + FVector(-800, RandBigNumber, 0.0);
			FVector NewLocation_Backward = GhoulRef->GetActorLocation() + FVector(800, RandBigNumber, 0.0);

			switch (RandNum)
			{
			case(0):
				MoveToLocation(NewLocation_Forward);
				MovingToAPoint = true;
				break;
			case(1):
				MoveToLocation(NewLocation_Backward);
				MovingToAPoint = true;
				break;
			case(2):
				MoveToLocation(NewLocation_Right);
				MovingToAPoint = true;
				break;
			case(3):
				MoveToLocation(NewLocation_Left);
				MovingToAPoint = true;
				break;
			default:
				break;
			}



		}

		/*if (GhoulMovingAroundSound) {

			FTimerHandle PlaySoundTimer;
			GetWorldTimerManager().SetTimer(PlaySoundTimer, [&]() {UGameplayStatics::PlaySoundAtLocation(GhoulRef, GhoulMovingAroundSound, GhoulRef->GetActorLocation()); }, 2.0, false);

		}*/

	}

}


void AGhoul_AIController::Attack(AGhoul_Character* GhoulRef, int32 AttackNumber, UAnimMontage* AttackOne, UAnimMontage* AttackTwo, UAnimMontage* AttackThree)
{

	if (GhoulRef) {

		if (AttackOne && AttackNumber == 0) {

			GhoulRef->PlayAnimMontage(AttackOne);

		}
		else if (AttackTwo && AttackNumber == 1) {

			GhoulRef->PlayAnimMontage(AttackTwo);

		}
		else if (AttackThree && AttackNumber == 2) {

			GhoulRef->PlayAnimMontage(AttackThree);

		}


	}


}