// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MiddleFenceGameMode.h"
#include "Characters/Base/MiddleFenceCharacterBase.h"
#include "Player/MiddleFencePlayerControllerBase.h"

AMiddleFenceGameMode::AMiddleFenceGameMode(const FObjectInitializer& ObjectInitilizer)
{
	PlayerControllerClass = AMiddleFencePlayerControllerBase::StaticClass();
	DefaultPawnClass = AMiddleFenceCharacterBase::StaticClass();


}

void AMiddleFenceGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages)
{
	Super::InitGame(MapName, Options, ErrorMessages);

	UE_LOG(LogTemp, Warning, TEXT("Middle Fence Game Mode base Init, Map Name : %s, Options : %s, ErrorMessages : %s"), *MapName, *Options, *ErrorMessages);
}
