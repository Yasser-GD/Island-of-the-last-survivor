// Fill out your copyright notice in the Description page of Project Settings.


#include "Item_Base_Actor.h"

#include "TestingMovemenntsCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Survivor_Character.h"



// Sets default values
AItem_Base_Actor::AItem_Base_Actor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(" Static Mesh"));
	RootComponent = ItemMesh;

}

// Called when the game starts or when spawned
void AItem_Base_Actor::BeginPlay()
{
	Super::BeginPlay();

	//CharacterRefCasting = Cast<ATestingMovemenntsCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	InitializePickUp();

}

// Called every frame
void AItem_Base_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AItem_Base_Actor::InitializePickUp()
{

	if (ItemDataTable) {

		const FItemData* ItemDataRef = ItemDataTable->FindRow<FItemData>(DesiredID, DesiredID.ToString());

		ItemStoringType = ItemDataRef->ItemStoringType;
		ItemType = ItemDataRef->ItemType;
		ItemStatices = ItemDataRef->ItemStatices;
		ItemAssetData = ItemDataRef->ItemAssetData;

		ItemMesh->SetStaticMesh(ItemAssetData.ItemMesh_ItemData);
		ItemMesh->SetWorldScale3D(ItemDataRef->ItemAssetData.DesiredScale);
		ItemMesh->SetSimulatePhysics(true);

	}

}

AItem_Base_Actor* AItem_Base_Actor::CreateNewItemCopy()
{

	NewItemReferance = NewObject<AItem_Base_Actor>(StaticClass());

	NewItemReferance->ItemStoringType = ItemStoringType;
	NewItemReferance->ItemAssetData = ItemAssetData;
	NewItemReferance->ItemStatices = ItemStatices;
	NewItemReferance->ItemType = ItemType;
	NewItemReferance->Quantity = Quantity;
	NewItemReferance->DesiredID = DesiredID;

	return NewItemReferance;

}

void AItem_Base_Actor::Use(ASurvivor_Character* CharacterRef)
{

  //----------------------- This is for eating food functionlity -----------------------------
	
	float RestoreAndHunger = ItemStatices.RestorationAmount + CharacterRef->Hunger;

	if (CharacterRef->Hunger != 100 && ItemType == EItemType::Consumable) {

		if (RestoreAndHunger > 100) {

			CharacterRef->Hunger += 100 - CharacterRef->Hunger;
			GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Hunger : %f "), CharacterRef->Hunger));
			CharacterRef->OnHungerChange_Del.Broadcast();

		}
		else {

			CharacterRef->Hunger += ItemStatices.RestorationAmount;
			CharacterRef->OnHungerChange_Del.Broadcast();

		}

	}


	if (Quantity > 1) {
		Quantity -= 1;
	}
	else {

		UActorComponent* InventoryRef = CharacterRef->GetComponentByClass(UInventory_System_AC::StaticClass());
		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);
		if (Inventory) {

			for (int i = 0; i < Inventory->Item_StoringArray.Num(); i++)
			{
				if (Inventory->Item_StoringArray[i]->DesiredID == DesiredID) {

					Inventory->Item_StoringArray.RemoveAt(i);
					GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("removed The Consumable Item from Inventory ")));
				}

			}

		}

		Destroy();

	}


}


void AItem_Base_Actor::DropItem(ASurvivor_Character* CharacterRef, TSubclassOf<AItem_Base_Actor> DroppedNewItem)
{

	UActorComponent* InventoryRef = CharacterRef->GetComponentByClass(UInventory_System_AC::StaticClass());

	bool IsUnStorable_Item = false;
	bool StillCarrying_UnStorableItem = false;


	if (InventoryRef) {

		GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Inventory Static Ref Is Valid ")));
		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);

		if (Inventory) {

			for (int i = 0; i < Inventory->Item_StoringArray.Num(); i++) {

				if (Inventory->Item_StoringArray[i]->DesiredID == DesiredID) {

					FVector SpawnLocation = CharacterRef->GetActorLocation();
					SpawnLocation.X += 100;
					FRotator SpawnRotation = CharacterRef->GetActorRotation();

					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

					//To Spawn A New Item And Decrease The Quantity of the item in the Array of the stored items 
					if (DroppedNewItem) {

						AItem_Base_Actor* SpawnedItemRef = CharacterRef->GetWorld()->SpawnActor<AItem_Base_Actor>(DroppedNewItem, SpawnLocation, SpawnRotation, SpawnParams);
						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("The Dropped Item Referrance is valid ")));

						if (SpawnedItemRef) {

							SpawnedItemRef->ItemStoringType = Inventory->Item_StoringArray[i]->ItemStoringType;
							SpawnedItemRef->ItemAssetData = Inventory->Item_StoringArray[i]->ItemAssetData;
							SpawnedItemRef->ItemStatices = Inventory->Item_StoringArray[i]->ItemStatices;
							SpawnedItemRef->ItemType = Inventory->Item_StoringArray[i]->ItemType;
							SpawnedItemRef->Quantity = 0;
							//SpawnedItemRef->Quantity -= 1;

							//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Quantity of the item in the storing array : %i "), Inventory->Item_StoringArray[i]->Quantity));
							SpawnedItemRef->DesiredID = Inventory->Item_StoringArray[i]->DesiredID;

							SpawnedItemRef->ItemMesh->SetStaticMesh(Inventory->Item_StoringArray[i]->ItemAssetData.ItemMesh_ItemData);

							SpawnedItemRef->ItemMesh->SetWorldScale3D(Inventory->Item_StoringArray[i]->ItemAssetData.DesiredScale);


							// To Decrease The Quantity
							if (Inventory->Item_StoringArray.IsValidIndex(i)) {

								//To Say That the item that im decreasing its quantity is an unstorable item
								if (Inventory->Item_StoringArray[i]->ItemStoringType == EItemStoringType::CantBeStored) {
									IsUnStorable_Item = true;
								}

								if (Inventory->Item_StoringArray[i]->Quantity == 1) {

									if (Inventory->Item_StoringArray.Contains(Inventory->Item_StoringArray[i])) {

										Inventory->Item_StoringArray.RemoveAt(i);

										GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Removed The Item From The Array ")));

									}

								}
								else {
									Inventory->Item_StoringArray[i]->Quantity--;
								}

							}

						}

					}
					else {
						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("New Item To Drop Is NULL ")));
					}



					break;

				}

			}


			if (IsUnStorable_Item) {

				// To Handle The Items That Can't Be Stored
				for (int y = 0; y < Inventory->Items_DestoryingArray.Num(); y++) {

					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Destoryed the unstorable item : %s "), *Inventory->Items_DestoryingArray[y]->DesiredID.ToString()));
					//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Destoryed the unstorable item : %s "), *DesiredID.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Destoryed the unstorable item : %i "), Inventory->Items_DestoryingArray.Num()));

					if (Inventory->Items_DestoryingArray[y] && Inventory->Items_DestoryingArray[y]->DesiredID == DesiredID) {

						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Green, FString::Printf(TEXT("Destoryed the unstorable item  ")));
						Inventory->Items_DestoryingArray[y]->Destroy();
						Inventory->Items_DestoryingArray.RemoveAt(y);
						break;

					}

				}

				for (int i = 0; i < Inventory->Item_StoringArray.Num(); i++) {

					if (Inventory->Item_StoringArray[i]->ItemStoringType == EItemStoringType::CantBeStored) {
						StillCarrying_UnStorableItem = true;
					}

				}

				if (!StillCarrying_UnStorableItem) {

					CharacterRef->MovementType = CharacterRef->LastMovementState_HeWasIn;

				}

			}

		}

	}

}

void AItem_Base_Actor::AttackingFunctionilty()
{
	GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Attacking Funcion in item base actor ")));
}


void AItem_Base_Actor::Interactpure(ASurvivor_Character* CharacterRef)
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
						Destroy();
						GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory : %i "), Inventory->Item_StoringArray[i]->Quantity));

					}

				}

			}

			if (!ItemAlreadyExist) {

				int32 Index = Inventory->Item_StoringArray.Add(CreateNewItemCopy());
				GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Increased Quantity ")));
				
				Inventory->Item_StoringArray[Index]->Quantity++;
				Destroy();

			}

			//to add the referance of the actual item interacted with to be able to destory the item on using it
			Inventory->Items_DestoryingArray.Add(this);

			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Added Item in Inventory ")));


		}

	}

}


void AItem_Base_Actor::Interact_Imp_Implementation(ASurvivor_Character* CharacterRef)
{

}

void AItem_Base_Actor::Equip_Weapon(ASurvivor_Character* CharacterRef)
{
}

void AItem_Base_Actor::UnEquip_Weapon(ASurvivor_Character* CharacterRef)
{
}

void AItem_Base_Actor::Use_Weapon(ASurvivor_Character* CharacterRef)
{
}
