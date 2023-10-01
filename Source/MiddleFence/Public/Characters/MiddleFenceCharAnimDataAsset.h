// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Game/MiddleFenceDataTypes.h"
#include "MiddleFenceCharAnimDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UMiddleFenceCharAnimDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
		FCharacterAnimationData CharacterAnimationData;
};
