// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "MiddleFenceAICharacterBase.generated.h"

class AMiddleFenceAIControllerBase;

/************************************************************************/
/* AMiddleFenceAICharacterBase                                          */
/************************************************************************/
UCLASS()
class MIDDLEFENCE_API AMiddleFenceAICharacterBase : public AMiddleFenceAdvanceCharacter
{
	GENERATED_BODY()

public:

	AMiddleFenceAICharacterBase(const FObjectInitializer& ObjectInitializer);

public:
	
	UFUNCTION(BlueprintPure, Category = "AI")
	AMiddleFenceAIControllerBase* GetMiddleFenceAIController() const;
};
