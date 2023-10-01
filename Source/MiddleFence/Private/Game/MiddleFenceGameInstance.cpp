// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MiddleFenceGameInstance.h"
#include "AbilitySystemGlobals.h"

void UMiddleFenceGameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
