// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory_System_AC.h"

// Sets default values for this component's properties
UInventory_System_AC::UInventory_System_AC()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventory_System_AC::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventory_System_AC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



