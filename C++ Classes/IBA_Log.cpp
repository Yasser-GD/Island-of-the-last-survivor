// Fill out your copyright notice in the Description page of Project Settings.


#include "IBA_Log.h"

#include "TestingMovemenntsCharacter.h"
#include "Survivor_Character.h"


void AIBA_Log::Interactpure(ASurvivor_Character* CharacterRef)
{

	bool ItemAlreadyExist = false;
	bool AlreadyIncreased = false;

	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Interacted with a log ")));

	UActorComponent* InventoryRef = CharacterRef->GetComponentByClass(UInventory_System_AC::StaticClass());

	if (InventoryRef) {

		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);

		if (Inventory) {

			for (int i = 0; i < Inventory->Item_StoringArray.Num(); i++) {

				if (Inventory->Item_StoringArray[i]->DesiredID == DesiredID) {

					ItemAlreadyExist = true;

					if (Inventory->Item_StoringArray[i]->Quantity < Inventory->Item_StoringArray[i]->ItemStatices.MaxQuantity) {

						StoringIndex = i;
						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory : %i "), StoringIndex));

					}

				}

			}

			if (!ItemAlreadyExist) {

				StoringIndex = Inventory->Item_StoringArray.Add(CreateNewItemCopy());

				if (Inventory->Item_StoringArray[StoringIndex]->Quantity < 2) {

					Inventory->Item_StoringArray[StoringIndex]->Quantity++;

				}

				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Increased Quantity ")));
				AlreadyIncreased = true;

				if (Inventory->Item_StoringArray[StoringIndex]->ItemStoringType == EItemStoringType::Storable) {
					Destroy();
				}

			}

			// To Handle the Items that Cant be stored
			if (Inventory->Item_StoringArray.IsValidIndex(StoringIndex)) {

				// if he interacted with unstorable item so he attach it to its sockets
				if (ItemStoringType == EItemStoringType::CantBeStored) {

					if (Inventory->Item_StoringArray[StoringIndex]->Quantity < Inventory->Item_StoringArray[StoringIndex]->ItemStatices.MaxQuantity) {

						//to add the referance of the actual item interacted with to be able to destory the item on using it
						Inventory->Items_DestoryingArray.Add(this);

						if (!AlreadyIncreased) {
							Inventory->Item_StoringArray[StoringIndex]->Quantity++;
						}

						//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory ")));

						ItemMesh->SetSimulatePhysics(false);
						ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

						FAttachmentTransformRules AttachRules = FAttachmentTransformRules::SnapToTargetIncludingScale;

						if (Inventory->Item_StoringArray[StoringIndex]->Quantity == 1) {

							//To Store the last movement state the player were in 
							if (CharacterRef->MovementType != EMovementType_SC::CarryingBuildItem) {
								CharacterRef->LastMovementState_HeWasIn = CharacterRef->MovementType;
							}

							
							if (!alreadyBroadcasted) {
								//To Give Instructions for the player on picking up his first Unstorable item
								OnPickingUp_UnstorableItem_Del.Broadcast();
								alreadyBroadcasted = true;
							}
							



							CharacterRef->MovementType = EMovementType_SC::CarryingBuildItem;
							ItemMesh->AttachToComponent(CharacterRef->GetMesh(), AttachRules, FName("Logs Hold Socket"));

						}
						else if (Inventory->Item_StoringArray[StoringIndex]->Quantity == 2) {
							CharacterRef->MovementType = EMovementType_SC::CarryingBuildItem;
							ItemMesh->AttachToComponent(CharacterRef->GetMesh(), AttachRules, FName("Second Log Hold Socket"));

						}


					}

				}
				// if he interacted with a storable item 
				else if (ItemStoringType == EItemStoringType::Storable && Inventory->Item_StoringArray[StoringIndex]->DesiredID == DesiredID) {


					if (Inventory->Item_StoringArray[StoringIndex]->Quantity < Inventory->Item_StoringArray[StoringIndex]->ItemStatices.MaxQuantity) {

						//to add the referance of the actual item interacted with to be able to destory the item on using it
						//Inventory->Items_DestoryingArray.Add(this);

						if (!AlreadyIncreased) {

							if (Quantity == 0) {
								Inventory->Item_StoringArray[StoringIndex]->Quantity++;
								GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Increased Item Quantity  ")));
								Destroy();
							}
							else if (Quantity > 1) {

								if (Inventory->Item_StoringArray[StoringIndex]->Quantity + Quantity > Inventory->Item_StoringArray[StoringIndex]->ItemStatices.MaxQuantity) {

									int SubstructionNum = Inventory->Item_StoringArray[StoringIndex]->ItemStatices.MaxQuantity -= Inventory->Item_StoringArray[StoringIndex]->Quantity;
									Inventory->Item_StoringArray[StoringIndex]->Quantity = 10;
									Quantity -= SubstructionNum;

								}
								else {

									Inventory->Item_StoringArray[StoringIndex]->Quantity += Quantity;
									Destroy();

								}


							}

						}

						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Item interacted with quantity : %i "), Quantity));


					}
					else if (Quantity == Inventory->Item_StoringArray[StoringIndex]->ItemStatices.MaxQuantity) {
						Destroy();
					}



				}




			}

		}

	}


}