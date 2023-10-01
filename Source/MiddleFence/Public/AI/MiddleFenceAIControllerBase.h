// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MiddleFenceAIControllerBase.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API AMiddleFenceAIControllerBase : public AAIController
{
	GENERATED_BODY()
	
protected:
	void OnPossess(APawn* InPawn) override;


	void OnUnPossess() override;

};
