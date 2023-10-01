// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MiddleFenceGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UMiddleFenceGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
};
