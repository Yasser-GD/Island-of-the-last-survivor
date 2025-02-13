// Fill out your copyright notice in the Description page of Project Settings.


#include "Building_System_Base_AC.h"

#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TestingMovemenntsCharacter.h"
#include "Camera/CameraComponent.h"
#include "CPP_Game_Ins_tance.h"
#include "Item_Base_Actor.h"
#include "Survivor_Character.h"


// Sets default values for this component's properties
UBuilding_System_Base_AC::UBuilding_System_Base_AC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MeshToPlace = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh To Place"));

	// ...
}


// Called when the game starts
void UBuilding_System_Base_AC::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn) {
		MainCharacter = Cast<ASurvivor_Character>(PlayerPawn);
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Blue, FString::Printf(TEXT("The Player Pawn is Vaild")));
	}

	GameInstanceRef = Cast<UCPP_Game_Ins_tance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (MainCharacter) {
		MainCharacter->OnRemovingSomething_Pressed_Del.AddDynamic(this, &UBuilding_System_Base_AC::RemoveBuilding_Pressed);
		MainCharacter->OnRemovingSomething_Released_Del.AddDynamic(this, &UBuilding_System_Base_AC::RemoveBuilding_Released);

		MainCharacter->OnBuildingSystemON_OFF_Del.AddDynamic(this, &UBuilding_System_Base_AC::BuildingSystem_ONOFF);
	}

	Owner = GetOwner();
	// ...
	BindToInput();
}


// Called every frame
void UBuilding_System_Base_AC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GameInstanceRef->IsBuildingSystem_ON) {
		TraceToGet_AHitLocation();

	}

	// ...
}

void UBuilding_System_Base_AC::BindToInput()
{
	/*
	Owner->InputComponent = NewObject<UInputComponent>(this);
	Owner->InputComponent->RegisterComponent();
	if (Owner->InputComponent)
	{
		// Bind inputs here
		Owner->EnableInput(GetWorld()->GetFirstPlayerController());
		Owner->InputComponent->BindAction("Building Mode", IE_Pressed, this, &UBuilding_System_Base_AC::BuildingSystem_ONOFF);
		// etc...
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Blue, FString::Printf(TEXT("Input Component Is Vaild")));


	}

	*/

}



void UBuilding_System_Base_AC::BuildingSystem_ONOFF(bool IsBuildingSystemON)
{


	/*if (IsBuildingSystemON) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Blue, FString::Printf(TEXT("The input is pressed")));
		MeshToPlace->OnComponentBeginOverlap.AddDynamic(this, &UBuilding_System_Base_AC::UnablePlaceBuilding);
		MeshToPlace->OnComponentEndOverlap.AddDynamic(this, &UBuilding_System_Base_AC::AblePlaceBuilding);
	}
	else {
		MeshToPlace->OnComponentBeginOverlap.RemoveDynamic(this, &UBuilding_System_Base_AC::UnablePlaceBuilding);
		MeshToPlace->OnComponentEndOverlap.RemoveDynamic(this, &UBuilding_System_Base_AC::AblePlaceBuilding);
	}*/


}

void UBuilding_System_Base_AC::TraceToGet_AHitLocation()
{

	FVector Start = MainCharacter->FirstPersonCameraComponent->GetComponentLocation();
	FVector End = Start + (MainCharacter->FirstPersonCameraComponent->GetForwardVector() * 700);
	//MainCharacter->GetFirstPersonCameraComponent()->G
	//DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0, -1, 3);

	FHitResult OnHit;
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(MainCharacter);
	bool Hit = GetWorld()->LineTraceSingleByChannel(OnHit, Start, End, ECC_Destructible, ColParams);

	if (Hit) {
		//MeshToPlace->SetStaticMesh()
		MeshToPlace->SetWorldLocation(OnHit.Location);
		HitActor = OnHit.GetActor();

		if (OnHit.GetComponent()->ComponentHasTag(FName("Aligning Tag")) && Is_ID_EqualsID) {

			HitAnAlignmentComponent = true;
			AlignmentComponentHit = OnHit.GetComponent();
			MeshToPlace->SetWorldLocation(AlignmentComponentHit->GetComponentLocation());
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT(" Whats Got Hit : %s"),*OnHit.GetComponent()->GetName() ));

		}
		else {
			HitAnAlignmentComponent = false;
		}

	}

}

void UBuilding_System_Base_AC::RemoveBuilding()
{
	
	if (IsRemovingBuilding) {

		FVector Start = MainCharacter->FirstPersonCameraComponent->GetComponentLocation();
		FVector End = Start + (MainCharacter->FirstPersonCameraComponent->GetForwardVector() * 300);
		//MainCharacter->GetFirstPersonCameraComponent()->G
		DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 2.0, -1, 3);

		FHitResult OnHit;
		FCollisionQueryParams ColParams;
		ColParams.AddIgnoredActor(MainCharacter);
		bool Hit = GetWorld()->LineTraceSingleByChannel(OnHit, Start, End, ECC_Vehicle, ColParams);

		if (Hit) {

			if (OnHit.GetActor()->ActorHasTag("Removable")) {
				OnHit.GetActor()->Destroy();
			}

		}

	}
   
	
}

void UBuilding_System_Base_AC::RemoveBuilding_Pressed()
{
	
	GetWorld()->GetTimerManager().SetTimer(RemovingBuildingTimer, [&]() {IsRemovingBuilding = true;  RemoveBuilding(); }, 0.6, false);
	
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT(" Removing Building")));
	
}


void UBuilding_System_Base_AC::RemoveBuilding_Released()
{
	
	if (MainCharacter && !IsRemovingBuilding && MainCharacter->MovementType == EMovementType_SC::CarryingBuildItem) {

		if (MainCharacter->GetInventory_AC() ) {

			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT(" Invetory Referance is valid ")));

			for (int i = 0; i < MainCharacter->GetInventory_AC()->Item_StoringArray.Num(); i++) {

				if (MainCharacter->GetInventory_AC()->Item_StoringArray[i]->ItemStoringType == EItemStoringType::CantBeStored ) {

					const FItemData* ItemDataRef = UnStorableItems_DataTable->FindRow<FItemData>(MainCharacter->GetInventory_AC()->Item_StoringArray[i]->DesiredID, MainCharacter->GetInventory_AC()->Item_StoringArray[i]->DesiredID.ToString());

					MainCharacter->GetInventory_AC()->Item_StoringArray[i]->DropItem(MainCharacter , ItemDataRef->ItemAssetData.NewItem_ToDrop );

				}

			}

		}

	}

	GetWorld()->GetTimerManager().ClearTimer(RemovingBuildingTimer);
	IsRemovingBuilding = false;


}


void UBuilding_System_Base_AC::UnablePlaceBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IsAbleToPlaceBuilding = false;
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT(" Overlapping so he can't setup a building")));
}

void UBuilding_System_Base_AC::AblePlaceBuilding(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsAbleToPlaceBuilding = true;
	GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Blue, FString::Printf(TEXT(" Able to place building")));
}

