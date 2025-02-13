// Fill out your copyright notice in the Description page of Project Settings.


#include "Axe_WeaponItem.h"

#include "TestingMovemenntsCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Damage_System_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "Damaging_Objects_Interface.h"
#include "Survivor_Character.h"


void AAxe_WeaponItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}


void AAxe_WeaponItem::Interactpure(ASurvivor_Character* CharacterRef)
{

	bool ItemAlreadyExist = false;

	UActorComponent* InventoryRef = CharacterRef->GetComponentByClass(UInventory_System_AC::StaticClass());

	if (InventoryRef) {

		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);

		if (Inventory) {

			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory ")));

			if (CharacterRef->MovementType == EMovementType_SC::NotHoldingAnyThing) {

				/*ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
				CharacterRef->MovementType = EMovementType::HoldingWeapon;
				AttachToComponent(CharacterRef->GetMesh(), AttachRules, EquipingWeaponSocketName);*/

				Equip_Weapon(CharacterRef);

				//To Assign the Weapon To An Empty Slot in the player

			}
			else {

				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
				//CharacterRef->MovementType = EMovementType::HoldingWeapon;
				AttachToComponent(CharacterRef->GetMesh(), AttachRules, UnEquipingWeaponSocketName);

			}

			// to assign the item to a slot if avaliable
			if (CharacterRef->Slot_1A == nullptr) {
				CharacterRef->Slot_1A = this;
				CharacterRef->alreadyEquiped_Slot_1 = true;
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Assigned the axe to slot 1 ")));
			}
			else if (CharacterRef->Slot_2A == nullptr) {
				CharacterRef->Slot_2A = this;
				CharacterRef->alreadyEquiped_Slot_2 = true;
			}
			else if (CharacterRef->Slot_3A == nullptr) {
				CharacterRef->Slot_3A = this;
				CharacterRef->alreadyEquiped_Slot_3 = true;
			}
			else if (CharacterRef->Slot_4A == nullptr) {
				CharacterRef->Slot_4A = this;
				CharacterRef->alreadyEquiped_Slot_4 = true;
			}

		}

	}

}

void AAxe_WeaponItem::AttackingFunctionilty()
{

	IsStartingDamage = true;
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Axe Got To Damaging ")));
	GetWorldTimerManager().SetTimer(HittingTimer, this, &AAxe_WeaponItem::FToMakeSweepHitSphere, 0.05, true);

}

void AAxe_WeaponItem::Equip_Weapon(ASurvivor_Character* CharacterRef)
{

	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetHiddenInGame(false);

	GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Axe Equiped ")));

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	CharacterRef->MovementType = EMovementType_SC::HoldingWeapon;
	AttachToComponent(CharacterRef->GetMesh(), AttachRules, EquipingWeaponSocketName);

	CharacterRef->OnUseItem.AddDynamic(this, &AAxe_WeaponItem::Use_Weapon);


	if (DesiredID == FName("Axe")) {
		CharacterRef->WeaponType = EWeaponType_SC::Axe;
	}
	else if (DesiredID == FName("Machete")) {
		CharacterRef->WeaponType = EWeaponType_SC::Machete;
	}
	else if (DesiredID == FName("Sword")) {
		CharacterRef->WeaponType = EWeaponType_SC::Sword;
	}

}

void AAxe_WeaponItem::UnEquip_Weapon(ASurvivor_Character* CharacterRef)
{

	if (CharacterRef->MovementType != EMovementType_SC::CarryingBuildItem) {

	}

	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetHiddenInGame(true);

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
	CharacterRef->MovementType = EMovementType_SC::NotHoldingAnyThing;
	AttachToComponent(CharacterRef->GetMesh(), AttachRules, FName("One Handed Melee Weapons"));

	CharacterRef->OnUseItem.RemoveDynamic(this, &AAxe_WeaponItem::Use_Weapon);

}

void AAxe_WeaponItem::Use_Weapon(ASurvivor_Character* CharacterRef)
{


	if (!CharacterRef->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && AttackAnim_1) {

		GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Attack 1 ")));
		CharacterRef->PlayAnimMontage(AttackAnim_1, PlayRate);

	}


	CharacterRef->CanMake_AnotherAttack = true;


}

void AAxe_WeaponItem::FToMakeSweepHitSphere()
{

	if (IsStartingDamage) {

		TArray<FHitResult> HitResultsArr;
		FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadious);

		FVector StartLoc = ItemMesh->GetSocketLocation(FName("AttackingWeaponsSocketName"));

		bool FHit = GetWorld()->SweepMultiByChannel(HitResultsArr, StartLoc, StartLoc, FQuat::Identity, ECC_Destructible, SphereShape);
		//DrawDebugSphere(GetWorld(), StartLoc, SphereRadious, 1, FColor::Red, false, 1.0);

		for (FHitResult OneHitResult : HitResultsArr) {

			if (OneHitResult.GetActor()->GetClass()->ImplementsInterface(UDamage_System_Interface::StaticClass()) && !ActorsGotHit.Contains(OneHitResult.GetActor())) {

				ActorsGotHit.Add(OneHitResult.GetActor());

				IDamage_System_Interface* DamageEnemyInterface = Cast<IDamage_System_Interface>(OneHitResult.GetActor());
				DamageEnemyInterface->Execute_EnemysTakingDamage_Imp(OneHitResult.GetActor(), ItemStatices.DamageAmount, OneHitResult.BoneName);
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Axe Damaged Something ")));

				UGameplayStatics::PlaySoundAtLocation(this, HitSound, OneHitResult.GetActor()->GetActorLocation());

				if (DamageEnemyInterface) {
					DamageEnemyInterface->TakeDamageAllEnemys(ItemStatices.DamageAmount, OneHitResult.BoneName);
					AlreadyHit = true;
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Axe Damaged Something ")));
				}

			}
			else if (OneHitResult.GetActor()->GetClass()->ImplementsInterface(UDamaging_Objects_Interface::StaticClass()) && !ActorsGotHit.Contains(OneHitResult.GetActor()))
			{

				ActorsGotHit.Add(OneHitResult.GetActor());

				IDamaging_Objects_Interface* Damage_Objects_Interface = Cast<IDamaging_Objects_Interface>(OneHitResult.GetActor());
			    
				Damage_Objects_Interface->Execute_Objects_TakingDamage_Imp(OneHitResult.GetActor(), ItemStatices.DamageAmount, OneHitResult.BoneName);
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Axe Damaged Something ")));


			}
		}



	}
	else {
		GetWorldTimerManager().ClearTimer(HittingTimer);
		ActorsGotHit.Empty();
	}



}

void AAxe_WeaponItem::OnDroppingItem(ASurvivor_Character* CharacterRef)
{
}



/*

void AAxe_WeaponItem::Interactpure(ATestingMovemenntsCharacter* CharacterRef)
{

	bool ItemAlreadyExist = false;

	UActorComponent* InventoryRef = CharacterRef->GetComponentByClass(UInventory_System_AC::StaticClass());

	if (InventoryRef) {

		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);

		if (Inventory) {


			for (int i = 0; i < Inventory->Item_StoringArray.Num(); i++) {

				if (Inventory->Item_StoringArray[i]->DesiredID == DesiredID) {

					ItemAlreadyExist = true;

					if (Inventory->Item_StoringArray[i]->Quantity < Inventory->Item_StoringArray[i]->ItemStatices.MaxQuantity) {

						Inventory->Item_StoringArray[i]->Quantity++;

						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory : %i "), Inventory->Item_StoringArray[i]->Quantity));

					}

				}

			}

			if (!ItemAlreadyExist) {

				int32 Index = Inventory->Item_StoringArray.Add(CreateNewItemCopy());
				Inventory->Item_StoringArray[Index]->Quantity++;

			}

			//to add the referance of the actual item interacted with to be able to destory the item on using it
			Inventory->Items_DestoryingArray.Add(this);

			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory ")));

			if (CharacterRef->MovementType == EMovementType::NotHoldingAnyThing) {

				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
				CharacterRef->MovementType = EMovementType::HoldingWeapon;
				AttachToComponent(CharacterRef->GetMesh(), AttachRules, EquipingWeaponSocketName);

				//To Assign the Weapon To An Empty Slot in the player

			}
			else {

				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;
				//CharacterRef->MovementType = EMovementType::HoldingWeapon;
				AttachToComponent(CharacterRef->GetMesh(), AttachRules, UnEquipingWeaponSocketName);

			}

			// to assign the item to a slot if avaliable
			if (CharacterRef->Slot_1A == nullptr) {
				CharacterRef->Slot_1A = this;
				CharacterRef->alreadyEquiped_Slot_1 = true;
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Assigned the axe to slot 1 ")));
			}
			else if (CharacterRef->Slot_2A == nullptr) {
				CharacterRef->Slot_2A = this;
				CharacterRef->alreadyEquiped_Slot_2 = true;
			}
			else if (CharacterRef->Slot_3A == nullptr) {
				CharacterRef->Slot_3A = this;
				CharacterRef->alreadyEquiped_Slot_3 = true;
			}
			else if (CharacterRef->Slot_4A == nullptr) {
				CharacterRef->Slot_4A = this;
				CharacterRef->alreadyEquiped_Slot_4 = true;
			}

		}

	}

}

*/