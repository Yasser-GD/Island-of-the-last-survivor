// Fill out your copyright notice in the Description page of Project Settings.


#include "Survivor_Character.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractInter_face.h"
#include "Kismet/GameplayStatics.h"
#include "CPP_Game_Ins_tance.h"

#include "Weapons_Interface.h"
#include "Inventory_System_AC.h"
#include "Item_Base_Actor.h"


// Sets default values
ASurvivor_Character::ASurvivor_Character()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetMesh(), FName("Neck"));
	//FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	MovementType = EMovementType_SC::NotHoldingAnyThing;

}

// Called when the game starts or when spawned
void ASurvivor_Character::BeginPlay()
{
	Super::BeginPlay();

	BaseCameraLoc = FirstPersonCameraComponent->GetComponentLocation();

	this->EnableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	GameInstanceRef = Cast<UCPP_Game_Ins_tance>(UGameplayStatics::GetGameInstance(GetWorld()));

	MovementType = EMovementType_SC::NotHoldingAnyThing;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	FTimerHandle HangerDecreaseTimer;
	GetWorldTimerManager().SetTimer(HangerDecreaseTimer, this, &ASurvivor_Character::HungerDecreasing, 5.0, true);


	InitialHealth = PlayerHealth;

	// TO Make the building system actor spawn into the world
	BuildingSystem_ON();

}

// Called every frame
void ASurvivor_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASurvivor_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &ASurvivor_Character::OnPrimaryAction);
	PlayerInputComponent->BindAction("PrimaryAction", IE_Released, this, &ASurvivor_Character::OnReleaseAction);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ASurvivor_Character::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ASurvivor_Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ASurvivor_Character::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ASurvivor_Character::LookUpAtRate);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASurvivor_Character::Interact_PressE);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ASurvivor_Character::Running_Pressed);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ASurvivor_Character::Running_Released);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASurvivor_Character::Dash);

	PlayerInputComponent->BindAction("Building Mode", IE_Pressed, this, &ASurvivor_Character::BuildingSystem_ON);

	PlayerInputComponent->BindAction("Building Mode OFF", IE_Pressed, this, &ASurvivor_Character::BuildingSystem_OFF);

	PlayerInputComponent->BindAction("Remove Something", IE_Pressed, this, &ASurvivor_Character::OnRemovingSomething_Pressed);
	PlayerInputComponent->BindAction("Remove Something", IE_Released, this, &ASurvivor_Character::OnRemovingSomething_Releasing);


	//Equiping Slots 1 , 2 , 3 , 4
	PlayerInputComponent->BindAction("Slot 1", IE_Pressed, this, &ASurvivor_Character::EquipItem_Slot_1);
	PlayerInputComponent->BindAction("Slot 2", IE_Pressed, this, &ASurvivor_Character::EquipItem_Slot_2);
	PlayerInputComponent->BindAction("Slot 3", IE_Pressed, this, &ASurvivor_Character::EquipItem_Slot_3);
	PlayerInputComponent->BindAction("Slot 4", IE_Pressed, this, &ASurvivor_Character::EquipItem_Slot_4);

}


void ASurvivor_Character::OnPlayerTakeDamage(float DamageTaken)
{

	if (!IsPlayerDead) {

		PlayerHealth -= DamageTaken;
		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Player Got Damaged : %f "), PlayerHealth));

		OnTakingDamage_Del.Broadcast();
		OnHealth_Update_Del.Broadcast();

		if (PlayerHealth <= 0) {
			IsPlayerDead = true;
			OnPlayerDeath.Broadcast();
			MovementType = EMovementType_SC::NotHoldingAnyThing;
		}

	}

}

void ASurvivor_Character::EquipItem_Slot_1()
{

	if (Slot_1A) {

		if (Slot_1A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass()) && MovementType != EMovementType_SC::CarryingBuildItem) {

			IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_1A);

			//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Actor in slot one has the interface ")));

			if (Weapons_InterfaceRef && !alreadyEquiped_Slot_1) {
				UnEquipItems_ToSlots();
				Weapons_InterfaceRef->Equip_Weapon(this);
				alreadyEquiped_Slot_1 = true;
			}
			else {
				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_1 = false;
			}


		}
		else if (MovementType == EMovementType_SC::CarryingBuildItem) {

			IsCarring_UnStorableItem();

		}

	}


}

void ASurvivor_Character::EquipItem_Slot_2()
{

	if (Slot_2A) {

		if (Slot_2A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass()) && MovementType != EMovementType_SC::CarryingBuildItem) {

			IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_2A);

			if (Weapons_InterfaceRef && !alreadyEquiped_Slot_2) {
				UnEquipItems_ToSlots();
				Weapons_InterfaceRef->Equip_Weapon(this);
				alreadyEquiped_Slot_2 = true;
			}
			else {
				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_2 = false;
			}

			//OnActorBeginOverlap.
		}
		else if (MovementType == EMovementType_SC::CarryingBuildItem) {

			IsCarring_UnStorableItem();

		}

	}

}

void ASurvivor_Character::EquipItem_Slot_3()
{

	if (Slot_3A) {

		if (Slot_3A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass()) && MovementType != EMovementType_SC::CarryingBuildItem) {

			IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_3A);

			if (Weapons_InterfaceRef && !alreadyEquiped_Slot_3) {
				UnEquipItems_ToSlots();
				Weapons_InterfaceRef->Equip_Weapon(this);
				alreadyEquiped_Slot_3 = true;
			}
			else {
				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_3 = false;
			}

		}
		else if (MovementType == EMovementType_SC::CarryingBuildItem) {

			IsCarring_UnStorableItem();

		}

	}

}

void ASurvivor_Character::EquipItem_Slot_4()
{

	if (Slot_4A) {

		if (Slot_4A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass()) && MovementType != EMovementType_SC::CarryingBuildItem) {

			IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_4A);

			if (Weapons_InterfaceRef && !alreadyEquiped_Slot_4) {
				UnEquipItems_ToSlots();
				Weapons_InterfaceRef->Equip_Weapon(this);
				alreadyEquiped_Slot_4 = true;
			}
			else {
				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_4 = false;
			}

		}
		else if (MovementType == EMovementType_SC::CarryingBuildItem) {

			IsCarring_UnStorableItem();

		}

	}

}


void ASurvivor_Character::UnEquipItems_ToSlots()
{

	if (alreadyEquiped_Slot_1) {

		if (Slot_1A) {

			if (Slot_1A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass())) {

				IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_1A);

				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Actor in slot one has the interface ")));

				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_1 = false;

			}

		}

	}
	else if (alreadyEquiped_Slot_2) {

		if (Slot_2A) {

			if (Slot_2A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass())) {

				IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_2A);

				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Actor in slot one has the interface ")));

				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_2 = false;

			}

		}

	}
	else if (alreadyEquiped_Slot_3) {

		if (Slot_3A) {

			if (Slot_3A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass())) {

				IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_3A);

				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Actor in slot one has the interface ")));

				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_3 = false;

			}

		}

	}
	else if (alreadyEquiped_Slot_4) {

		if (Slot_4A) {

			if (Slot_4A->GetClass()->ImplementsInterface(UWeapons_Interface::StaticClass())) {

				IWeapons_Interface* Weapons_InterfaceRef = Cast<IWeapons_Interface>(Slot_4A);

				//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Actor in slot one has the interface ")));

				Weapons_InterfaceRef->UnEquip_Weapon(this);
				alreadyEquiped_Slot_4 = false;

			}

		}

	}

}

void ASurvivor_Character::OnPrimaryAction()
{
	if (!IsPlayerDead && !GameInstanceRef->IsBuildingSystem_ON) {

		OnUseItem.Broadcast(this);
	}
	// Trigger the OnItemUsed Event

}

void ASurvivor_Character::OnReleaseAction()
{
	if (!IsPlayerDead && !GameInstanceRef->IsBuildingSystem_ON) {
		OnReleaseItem.Broadcast();
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Mouse Released ")));

	}
}

void ASurvivor_Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, FString::Printf(TEXT("Begin Touchhhhhh")));
		OnPrimaryAction();
	}

	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;

}

void ASurvivor_Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ASurvivor_Character::MoveForward(float Value)
{
	if (!IsPlayerDead && Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASurvivor_Character::MoveRight(float Value)
{
	if (!IsPlayerDead && Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASurvivor_Character::TurnAtRate(float Rate)
{
	if (!IsPlayerDead) {
		// calculate delta for this frame from the rate information
		AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());

	}
}

void ASurvivor_Character::LookUpAtRate(float Rate)
{
	if (!IsPlayerDead) {
		// calculate delta for this frame from the rate information
		//GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("Look Up Rate : %f " ), Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds()));
		AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());

	}
}


void ASurvivor_Character::ToLineToTryGetHeight()
{
	if (!IsPlayerDead) {
		//GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("He Got into the function ")));
		FHitResult HitResult;
		FVector Start = GetActorLocation();
		FVector End = Start + GetActorForwardVector() * 600;
		bool Hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Destructible);
		if (Hit) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 2);
			if (HitResult.GetActor()->ActorHasTag("Wall")) {

				//GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("Box Bounds : %s"), *HitResult.GetActor()->GetActorTransform().ToString()));
				DrawDebugLine(GetWorld(), Start, End, FColor::Cyan, false, 2, -1, 3);

			}
		}

	}

}

void ASurvivor_Character::Running_Pressed()
{

	if (!IsPlayerDead) {

		/*GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("He Dasheeed")));
		FVector DashSpeed = GetVelocity() * 6;
		GetCharacterMovement()->Launch(DashSpeed);*/

		//GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		
		/////////////////
		GetWorldTimerManager().ClearTimer(SlowingDownTimer);

		GetWorldTimerManager().SetTimer(SpeedingTime, [&]()
			{
				if (GetCharacterMovement()->MaxWalkSpeed < RunSpeed) {

					GetCharacterMovement()->MaxWalkSpeed += 50;
					
				}
				else {
					GetWorldTimerManager().ClearTimer(SpeedingTime);
				}

			}, 0.1, true);


	}

}

void ASurvivor_Character::Running_Released()
{

	if (!IsPlayerDead) {

		GetWorldTimerManager().ClearTimer(SpeedingTime);

		GetWorldTimerManager().SetTimer(SlowingDownTimer, [&]()
			{
				if (GetCharacterMovement()->MaxWalkSpeed > WalkSpeed) {

					GetCharacterMovement()->MaxWalkSpeed -= 50;
				  //GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("Slowing Down")));
				}
				else {
					GetWorldTimerManager().ClearTimer(SlowingDownTimer);
				}

			}, 0.15, true);


		


	}

}

void ASurvivor_Character::Dash()
{

	GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("He Dasheeed : %s "), *GetVelocity().ToString() ));

	FVector Velocity;
	Velocity.X = GetVelocity().X / GetVelocity().X;
	Velocity.Y = GetVelocity().Y / GetVelocity().Y;

	GetCharacterMovement()->AddImpulse(Velocity * 200);


	//GetWorldTimerManager().SetTimer(SpeedingTime, [&]() {}, 0.1, true);

}


void ASurvivor_Character::HungerDecreasing()
{

	Hunger -= 1.0;
	//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Hunger in the player  : %f "), Hunger));

	OnHungerChange_Del.Broadcast();

	if (Hunger >= 90 && Hunger <= 100) {

		GetWorldTimerManager().SetTimer(HealthRestorationTimer, this, &ASurvivor_Character::HealthRestoration_FromHunger, 0.35, true);

	}

	if (Hunger < 0) {
		OnPlayerTakeDamage(5.0);
	}

}

void ASurvivor_Character::HealthRestoration_FromHunger()
{

	if (PlayerHealth < InitialHealth) {

		PlayerHealth += 1;
		OnHealth_Update_Del.Broadcast();
		//GEngine->AddOnScreenDebugMessage(-1, 6, FColor::Red, FString::Printf(TEXT("Player Health : %f "), PlayerHealth));

	}

	if (Hunger < 90) {
		GetWorldTimerManager().ClearTimer(HealthRestorationTimer);
	}


}


void ASurvivor_Character::Interact_PressE()
{

	if (!IsPlayerDead) {
		//GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("Pressed E")));
		FVector Start = FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + (FirstPersonCameraComponent->GetForwardVector() * 700);

		//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 4.0, -1, 3);

		FHitResult OnHit;
		FCollisionQueryParams ColParams;
		bool Hit = GetWorld()->LineTraceSingleByChannel(OnHit, Start, End, ECC_Destructible, ColParams);

		if (Hit) {

			IInteractInter_face* Inter_face = Cast<IInteractInter_face>(OnHit.GetActor());

			if (OnHit.GetActor()->GetClass()->ImplementsInterface(UInteractInter_face::StaticClass())) {

				Inter_face->Execute_Interact_Imp(OnHit.GetActor(), this);

				if (Inter_face) {

					Inter_face->Interactpure(this);

					//Inter_face->Execute_Interact_Imp(OnHit.GetActor());
					//GEngine->AddOnScreenDebugMessage(-1, 20.0, FColor::Cyan, FString::Printf(TEXT("The Actor Has An Interface")));

				}

			}

		}

	}

}


void ASurvivor_Character::BuildingSystem_ON()
{

	if (BuildingMaster_Actor) {

		if (GameInstanceRef && !GameInstanceRef->IsBuildingSystem_ON && AlreadySpawnedBuildingMaster) {

			GameInstanceRef->IsBuildingSystem_ON = true;
			//GEngine->AddOnScreenDebugMessage(-1, 6.0, FColor::Green, FString::Printf(TEXT("He Turned the building System ON")));
			OnBuildingSystemON_OFF_Del.Broadcast(true);

		}
		else if (GameInstanceRef && GameInstanceRef->IsBuildingSystem_ON && AlreadySpawnedBuildingMaster) {

			GameInstanceRef->IsBuildingSystem_ON = false;
			IsBuilding_ON = false;
			OnBuildingSystemON_OFF_Del.Broadcast(false);

		}


		if (!AlreadySpawnedBuildingMaster) {

			FVector SpawnLocation = GetActorLocation();
			FRotator SpawnRotation = GetActorRotation();
			FActorSpawnParameters SpawnParam;
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnedActor = GetWorld()->SpawnActor<AActor>(BuildingMaster_Actor, SpawnLocation, SpawnRotation, SpawnParam);

			AlreadySpawnedBuildingMaster = true;
			SpawnedActor->SetActorScale3D(FVector::ZeroVector);

		}


	}


}

void ASurvivor_Character::BuildingSystem_OFF()
{
	if (GameInstanceRef) {

		GameInstanceRef->IsBuildingSystem_ON = false;
		IsBuilding_ON = false;
		OnBuildingSystemON_OFF_Del.Broadcast(false);

	}

}


void ASurvivor_Character::IsCarring_UnStorableItem()
{

	if (MovementType == EMovementType_SC::CarryingBuildItem) {

		bool IsCarringUnStorableItem = false;

		for (int32 i = 0; i < GetInventory_AC()->Item_StoringArray.Num(); i++) {

			if (GetInventory_AC()->Item_StoringArray[i]->ItemStoringType == EItemStoringType::CantBeStored) {
				IsCarringUnStorableItem = true;
			}
		}

		if (!IsCarringUnStorableItem) {
			MovementType = LastMovementState_HeWasIn;
		}

	}

}



void ASurvivor_Character::OnRemovingSomething_Pressed()
{
	OnRemovingSomething_Pressed_Del.Broadcast();
}


void ASurvivor_Character::OnRemovingSomething_Releasing()
{
	OnRemovingSomething_Released_Del.Broadcast();
}


UInventory_System_AC* ASurvivor_Character::GetInventory_AC()
{

	UActorComponent* InventoryRef = this->GetComponentByClass(UInventory_System_AC::StaticClass());

	if (InventoryRef) {

		UInventory_System_AC* Inventory = Cast<UInventory_System_AC>(InventoryRef);

		if (Inventory) {

			return Inventory;

		}
		else {
			return nullptr;
		}

	}
	else {
		return nullptr;
	}


}


