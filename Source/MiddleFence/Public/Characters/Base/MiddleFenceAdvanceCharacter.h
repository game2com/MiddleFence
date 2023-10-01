// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/MiddleFenceCharacterBase.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "Characters/MiddleFenceCharacterDataAsset.h"
#include "Components/ChainInputComponent.h"
#include "MiddleFenceAdvanceCharacter.generated.h"

class UMiddleFenceAbilitySysComponent;
class UMiddleFenceAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

class AMiddleFenceInventoryBase;



DECLARE_MULTICAST_DELEGATE(FDelegateOnActionAnimationPossibleEnd)
DECLARE_MULTICAST_DELEGATE(FDelegateOnActionAnimationEnd)
DECLARE_MULTICAST_DELEGATE(FDelegateOnActionInputEnabled)

/************************************************************************/
/* the Advance character class has advance features like Gameplay Ability System and AttributeSet.
/* This class is responsible for inventory items.
/* Character startup effects and abilities are applied here.
/************************************************************************/

UCLASS()
class MIDDLEFENCE_API AMiddleFenceAdvanceCharacter : public AMiddleFenceCharacterBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMiddleFenceAdvanceCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());



protected:


	/************************************************************************/
	/* Motion Warping														*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainCharacter Essential Components")
		class UMotionWarpingComponent* MotionWarpingComponent;


	/************************************************************************************************/
	/* All character essential data like Effects, Abilities or animations are store in this file	*/
	/************************************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
		UMiddleFenceCharacterDataAsset* CharacterDataAsset;

	/************************************************/
	/* Store character data							*/
	/************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
		FCharacterData CharacterData;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
		UAnimMontage* TestHitAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Health)
		UAnimMontage* TestDeathAnim;


public:
	/************************************************************************/
	/* Get MotionWarpingComponent                                           */
	/************************************************************************/
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

	UFUNCTION(BlueprintCallable, Category = "Motion Warping")
	void UpdateMotionWarpingOnTarget(FName AnimationWarpTargetName, float PositionOffset);

#pragma region General Methods
public:

	/************************************************************************/
	/* A good place to fill CharacterData.                                   */
	/************************************************************************/
	void PreInitializeComponents() override;

protected:

	/************************************************************************/
	/* A good place for spawning default inventory items.					*/
	/************************************************************************/
	void BeginPlay() override;


protected:

	/**************************************************************************/
	/*																		  */
	/* Giving default abilities and applying default effects take place here. */
	/*																		  */
	/**************************************************************************/
	virtual void PossessedBy(AController* NewController) override;


	//virtual void OnRep_PlayerState() override;


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion

#pragma region Ability System
protected:

	/************************************************************************/
	/* AbilitySystemComponent                                               */
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UMiddleFenceAbilitySysComponent* AbilitySystemComponent;

	/**************************************************************************************************************/
	/* [AttributeSetComponent] is responsible for character basic attribute like [Health], [MaxHealth] and so on. */
	/**************************************************************************************************************/
	UPROPERTY(Transient)
		UMiddleFenceAttributeSetBase* AttributeSetComponent;

private:
	/************************************************************************/
	/* Override methods of IAbilitySystemInterface							*/
	/************************************************************************/
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:

	UAbilitySystemComponent* GetMyAbilitySystemComponent() const;

protected:

	/****************************************************************************************************/
	/*																									*/
	/* Auto apply all Abilities that are assigned in CharacterData asset file at the start of the play  */
	/*																									*/
	/****************************************************************************************************/
	void GiveAbilities();


	/************************************************************************************************/
	/*																								*/
	/* Auto apply all effects that are set in CharacterData asset file at the start of the play		*/
	/*																								*/
	/************************************************************************************************/
	void ApplyStartupEffect();

public:

	/************************************************************************/
	/*Set [CharacterData] CharacterData										*/
	/************************************************************************/
	void SetCharacterData(const FCharacterData& data);

	/************************************************************************/
	/*return [CharacterData] CharacterData									*/
	/************************************************************************/
	FCharacterData GetCharacterData() const;


public:

	/******************************************************************************/
	/*																			  */
	/* A helper function to add effect to this character						  */
	/* [TSubclassOf<UGameplayEffect> Effect] A GameplayEffect class	or blueprint. */
	/*																			  */
	/******************************************************************************/
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect);

#pragma endregion

#pragma region Action Input
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input|Chain")
		UChainInputComponent* ChainInput;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		//TArray<FComboInput> PlayerActionInputBuffer;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UPROPERTY(EditAnywhere, Category = "Input|Chain")
		//float ActionInputClearTime;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	/*UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		bool bActionInputIsMatch;*/

	//UPROPERTY(EditAnywhere, Category = "Input|Chain")
		//FVector2D DelayInputAcceptRange;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	/*UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		bool bCanReceiveActionInput;*/

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		//bool NewActionInputReceivedWhenInventoryItemWasRunning = false;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//FTimerHandle TimerHandle_ResetActionInputOnDelay;

public:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	FDelegateOnActionAnimationPossibleEnd OnActionAnimationPossibleEnds;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	FDelegateOnActionAnimationEnd OnActionAnimationEnds;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//FDelegateOnActionInputEnabled OnActionInputEnabled;


protected:

	/************************************************************************/
	/* ****                                                                 */
	/************************************************************************/
	virtual void OnNewChainInputAddedToBuffer(EActionInputType newInput);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//void AddNewActionInputToBuffer(EActionInputType newInputType);

public:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UFUNCTION(BlueprintPure, Category = "Input|Chain")
	//bool CheckIfActionInputAccepted() const;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UFUNCTION(BlueprintPure, Category = "Input|Chain")
		//TArray<FComboInput> GetPlayerActionInputBuffer() const;

	/************************************************************************/
	/*******                                                                */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Input|Chain")
		virtual void ClearActionInputBuffer();

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//UFUNCTION(BlueprintCallable, Category = "Input|Chain")
		//void ToggleReceiveActionInput(bool canReceive);

protected:

	/************************************************************************/
	/* Last input's Added-Time should be updated otherwise the time between
		start of the animation till Possible End point calculated while
		the character can not add a new input during this time.
		
		In fact, we should calculate input delay when the animation 
		triggered the Possible End anim_notify.								*/
	/************************************************************************/
	//void UpdateLastActionInputTimeOnAnimationPossibleEnd();

	/************************************************************************/
	/*                                                                      */
	/* When an active inventory item's animation is finished then a timer
		runs to reset input buffer after a period of time if the player was
		inactive or didn't add a new action input to buffer.				*/
	/*                                                                      */
	/************************************************************************/
	//void ResetActionInputOnDelay();

protected:

	/************************************************************************/
	/* When the character added a new input to buffer then this function
		get called and search through all inventory items and checks if
		chain-input runs an action											*/
	/************************************************************************/
	virtual void ExecuteActionInput(TArray<FComboInput>InputBuffer);

#pragma endregion

#pragma region Health & Hit
protected:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		bool bIsDead;

protected:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnHealthChange(float Health, float MaxHealth, AActor*& whoSendDamage);

	void OnHealthChange_Implementation(float Health, float MaxHealth, AActor*& whoSendDamage);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnDeath();

	void OnDeath_Implementation();


public:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void OnKillEnemy(AActor* KilledActor);

#pragma endregion

#pragma region Inventory
protected:


	/************************************************************************/
	/* All default items that character acquire at the start of the play	*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
		TArray<TSubclassOf<AMiddleFenceInventoryBase>> InventoryDefaultItems;


	/************************************************************************/
	/* All spawned inventory items store in here							*/
	/************************************************************************/
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		//TArray<AMiddleFenceInventoryBase*> InventoryItems;


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
	TArray<FInventorySlot> InventoryItemsSlots;



	/************************************************************************/
	/*																		*/
	/* When an item on inventory is active and play animation or doing 
		something then this variable is true								*/
	/*																		*/
	/************************************************************************/
	//UPROPERTY(VisibleAnywhere, Category = "Inventory|Action")
		//bool InventoryItemIsRunning;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Action")
	AMiddleFenceInventoryBase* CurrentActiveWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory|Action")
	AMiddleFenceInventoryBase* LastActiveWeapon;
public:

	/************************************************************************/
	/*																		*/
	/* Spawn all items that are inside [InventoryDefaultItems].				*/
	/*																		*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SpawnInventoryDefaultItems();


	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsRightWeaponInventorySlotEmpty() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsLeftWeaponInventorySlotEmpty() const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	AMiddleFenceInventoryBase* GetCurrentActiveWeapon() const;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void AddNewItemToInventorySlot(FInventorySlot newItemSlot);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FInventorySlot> GetInventoryItemSlots() const;


	void SwitchWeapon(AMiddleFenceInventoryBase* newWeapon, AMiddleFenceInventoryBase* oldWeapon);

	void ForceCancelActiveWeapon();

	/************************************************************************/
	/* It Get called when an active item on inventory runs					*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void OnActionAnimationStart();


	/************************************************************************************/
	/* 																					*/
	/* For Example, a sword attack animation may takes time to finish but at the middle
		of that animation all functionalities are done and player can ignore rest of
		the animation and run another animation.
	/* Called from Anim_Notify															*/
	/*																					*/
	/************************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnActionAnimationPossibleEnd();


	/****************************************************************************/
	/*																			*/
	/* Directly get called from an inventory item when it's animation finished  */
	/*																			*/
	/****************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void OnActionAnimationEnd();


	/************************************************************************/
	/*																		*/
	/* Called from Anim_Notify                                              */
	/*																		*/
	/************************************************************************/
	//UFUNCTION(BlueprintCallable, Category = "Inventory")
	//void OnActionAnimationCanReceiveInput();

#pragma endregion



};
