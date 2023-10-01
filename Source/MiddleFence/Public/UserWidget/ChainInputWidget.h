// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Game/MiddleFenceDataTypes.h"
#include "ChainInputWidget.generated.h"



/**
 * 
 */
UCLASS()
class MIDDLEFENCE_API UChainInputWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Widget")
	void OnAddNewInputToBuffer(EActionInputType newInput);

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
		void OnUpdateLastInput(EActionInputType newInput);

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void OnClearInputBuffer();
};
