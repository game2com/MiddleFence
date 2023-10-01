// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "InputActionValue.h"
#include "MiddleFenceMainCharacterBase.generated.h"



/************************************************************************/
/* The base class for all playable main characters						*/
/* Good place for adding camera & input components						*/
/************************************************************************/
UCLASS()
class MIDDLEFENCE_API AMiddleFenceMainCharacterBase : public AMiddleFenceAdvanceCharacter
{
	GENERATED_BODY()

public:

	AMiddleFenceMainCharacterBase(const FObjectInitializer& ObjectInitializer);

#pragma region MainCharacter Essential Components

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainCharacter Essential Components")
		class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MainCharacter Essential Components")
		class USpringArmComponent* SpringArmComponent;

#pragma endregion



#pragma region EnhancedInput
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Chain|UI")
		TSubclassOf <class UChainInputWidget> ChainInputWidgetTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input|Chain|UI")
		class UChainInputWidget* ChainInputWidget;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_Turn;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_Shift;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_RightWeaponLightAttack;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_RightWeaponLightAttack_Hold;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_RightWeaponHeavyAttack;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_RightWeaponHeavyAttack_Hold;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_LeftWeaponLightAttack;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_LeftWeaponLightAttack_Hold;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_LeftWeaponHeavyAttack;

	UPROPERTY(EditDefaultsOnly, Category = Input)
		class UInputAction* InputAction_LeftWeaponHeavyAttack_Hold;

protected:

	void BeginPlay() override;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void PawnClientRestart() override;


	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SpawnChainInputWidget();

	void OnNewChainInputAddedToBuffer(EActionInputType newInput) override;
	void OnWaitLongTimeNoInput();
	void OnUpdateLastInput(EActionInputType newInput);
	virtual void ClearActionInputBuffer() override;
	void OnCancelInput();
#pragma endregion

#pragma region Movement Functionality

protected:

	/************************************************************************/
	/* MoveForward                                                          */
	/************************************************************************/
	void MoveForward(const FInputActionValue& Value) override;

	/************************************************************************/
	/* MoveRight                                                            */
	/************************************************************************/
	void MoveRight(const FInputActionValue& Value) override;

	/************************************************************************/
	/* OnTurn                                                               */
	/************************************************************************/
	void OnTurn(const FInputActionValue& Value);

	/************************************************************************/
	/* OnLookUp                                                             */
	/************************************************************************/
	void OnLookUp(const FInputActionValue& Value);

	void OnShiftKey(const FInputActionValue& Value);
#pragma endregion



#pragma region Combat System
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat System")
	bool HeavyAttackInputIsActive;


protected:


	/************************************************************************/
	/* RightWeaponLightAttack												*/
	/************************************************************************/
	void RightWeaponLightAttack(const FInputActionValue& Value);
	void RightWeaponLightAttackHold(const FInputActionValue& Value);
	void RightWeaponLightAttackRelease(const FInputActionValue& Value);

	void RightWeaponHeavyAttack(const FInputActionValue& Value);
	void RightWeaponHeavyAttackHold(const FInputActionValue& Value);
	void RightWeaponHeavyAttackReleased(const FInputActionValue& Value);

	void LeftWeaponLightAttack(const FInputActionValue& Value);
	void LeftWeaponLightAttackHold(const FInputActionValue& Value);
	void LeftWeaponLightAttackRelease(const FInputActionValue& Value);

	void LeftWeaponHeavyAttack(const FInputActionValue& Value);
	void LeftWeaponHeavyAttackHold(const FInputActionValue& Value);
	void LeftWeaponHeavyAttackRelease(const FInputActionValue& Value);

	virtual void ExecuteActionInput(TArray<FComboInput>InputBuffer) override;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	bool CheckTarget();

#pragma endregion




};
