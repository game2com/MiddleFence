// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Game/MiddleFenceDataTypes.h"
#include "MiddleFenceInventoryBase.generated.h"

UCLASS(HideCategories("Actor Tick", "Physics", "WorldPartition", "Cooking", "DataLayers", "HLOD"))
class MIDDLEFENCE_API AMiddleFenceInventoryBase : public AActor
{
	GENERATED_BODY()
	
public:	

	/************************************************************************/
	/* Default properties                                                   */
	/************************************************************************/
	AMiddleFenceInventoryBase();

	/************************************************************************/
	/* A simple root component												*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = Component)
		USceneComponent* SceneRootComponent;

	/************************************************************************/
	/* Socket name where item should spawn there like holster or ...       */
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
		FName CharacterAttachPointSocket;

protected:

	/************************************************************************/
	/* Specify the type of item. it is a weapon, potion or ...				*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, Category = "Setup")
		EInventoryItemType InventoryItemType;

	/************************************************************************/
	/* The position of this item on the inventory. Specify on spawn.			*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, Category = "Setup")
		EInventorySlotType InventorySlotType;	
	
	/************************************************************************/
	/* specify the input which will execute this item						*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, Category = "Setup")
		EActionInputType ActionTriggerKey;

	/************************************************************************/
	/* The ability class which will execute after this item get executed    */
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
		TSubclassOf<class UGameplayAbility> ItemAbilityClass;


	/************************************************************************/
	/* The tag which execute ability class									*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
		FGameplayTag AbilityEventTag;
	

public:

	/************************************************************************/
	/* Get called when this item spawned in the world.                       */
	/************************************************************************/
	virtual void OnAfterSpawning();

	/************************************************************************/
	/* return ItemAbilityClass												*/
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Gameplay Ability")
	TSubclassOf<class UGameplayAbility> GetItemAbilityClass() const;


	/************************************************************************/
	/* return InventoryItemType                                             */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Inventory")
	EInventoryItemType GetInventoryItemType() const;


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateInventoryItemType(EInventoryItemType newType);


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void UpdateInventorySlotType(EInventorySlotType newType);


	/************************************************************************/
	/* If it get called the current active item stop working.
	/* Animation, ability and etc will stop
	/************************************************************************/
	virtual void ForceCancel();


	/************************************************************************/
	/* Main function to check if this item can be executed.					*/
	/************************************************************************/
	virtual AMiddleFenceInventoryBase* OnAction(TArray<FComboInput>InputBuffer);


	/************************************************************************/
	/* When the item can run another thing. useful for items with animation
	/* get called with Anim_Notify											*/
	/************************************************************************/
	virtual void OnAnimationPossibleEnd();


	/************************************************************************/
	/* get called with Anim_Notify in the first section of animation to let
	/* the character added input											*/
	/************************************************************************/
	virtual void OnActionInputActivation();

};
