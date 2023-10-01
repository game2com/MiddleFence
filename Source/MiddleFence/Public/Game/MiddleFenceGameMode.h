// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MiddleFenceGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API AMiddleFenceGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	AMiddleFenceGameMode(const FObjectInitializer& ObjectInitilizer = FObjectInitializer::Get());

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages) override;
	
};
