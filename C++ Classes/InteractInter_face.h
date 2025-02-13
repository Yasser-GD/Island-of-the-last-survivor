// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInter_face.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractInter_face : public UInterface
{
	GENERATED_BODY()
};


class ATestingMovemenntsCharacter;

class TESTINGMOVEMENNTS_API IInteractInter_face
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

public:

	UFUNCTION()
    virtual	void Interactpure(ASurvivor_Character* CharacterRef) = 0;
     
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	  void Interact_Imp(ASurvivor_Character* CharacterRef);

};
