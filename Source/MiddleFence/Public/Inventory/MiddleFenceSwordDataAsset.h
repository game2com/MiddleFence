// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "MiddleFenceSwordDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UMiddleFenceSwordDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
		TArray< UDataTable*> ComboDataTables;
};
