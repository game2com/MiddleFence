// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/MiddleFenceInventoryBase.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"


AMiddleFenceInventoryBase::AMiddleFenceInventoryBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root Component"));
	RootComponent = SceneRootComponent;
}

void AMiddleFenceInventoryBase::OnAfterSpawning()
{
	//nothing
}


TSubclassOf<class UGameplayAbility> AMiddleFenceInventoryBase::GetItemAbilityClass() const
{
	return ItemAbilityClass;
}

EInventoryItemType AMiddleFenceInventoryBase::GetInventoryItemType() const
{
	return InventoryItemType;
}

void AMiddleFenceInventoryBase::UpdateInventoryItemType(EInventoryItemType newType)
{
	InventoryItemType = newType;
}

void AMiddleFenceInventoryBase::UpdateInventorySlotType(EInventorySlotType newType)
{
	InventorySlotType = newType;
}

void AMiddleFenceInventoryBase::ForceCancel()
{
	//nothing
}


AMiddleFenceInventoryBase* AMiddleFenceInventoryBase::OnAction(TArray<FComboInput>InputBuffer)
{
	if (InputBuffer.Num() == 0)
		return nullptr;

	if (!GetOwner())
		return nullptr;

	if (InputBuffer[0].Input == EActionInputType::None || InputBuffer[0].Input != ActionTriggerKey)
		return nullptr;

	FGameplayEventData payload;
	payload.Instigator = this;
	payload.EventTag = AbilityEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), AbilityEventTag, payload);

	return this;
}

void AMiddleFenceInventoryBase::OnAnimationPossibleEnd()
{
	//nothing
}

void AMiddleFenceInventoryBase::OnActionInputActivation()
{
	//nothing
}

