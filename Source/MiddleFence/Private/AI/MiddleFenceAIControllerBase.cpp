// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MiddleFenceAIControllerBase.h"

void AMiddleFenceAIControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	UE_LOG(LogTemp, Warning, TEXT("OnPossess %s"), *GetNameSafe(InPawn));
}

void AMiddleFenceAIControllerBase::OnUnPossess()
{
	Super::OnUnPossess();

	UE_LOG(LogTemp, Warning, TEXT("OnUnPossess"));
}
