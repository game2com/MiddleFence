// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/MiddleFenceDataTypes.h"
#include "MiddleFenceCharacterDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MIDDLEFENCE_API UMiddleFenceCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FCharacterData CharacterData;
};
