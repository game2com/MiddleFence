// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/MiddleFenceAICharacterBase.h"
#include "AI/MiddleFenceAIControllerBase.h"

AMiddleFenceAICharacterBase::AMiddleFenceAICharacterBase(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	AIControllerClass = AMiddleFenceAIControllerBase::StaticClass();
}

AMiddleFenceAIControllerBase* AMiddleFenceAICharacterBase::GetMiddleFenceAIController() const
{
	if (GetController() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("%s, GetController returns null!"), *GetNameSafe(this));
		return nullptr;
	}

	AMiddleFenceAIControllerBase* MDAI = Cast<AMiddleFenceAIControllerBase>(GetController());

	return MDAI;
}
