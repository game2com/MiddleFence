// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MiddleFenceCharacterBase.generated.h"


/********************************************************************************************/
/* The base class of all characters in the MiddleFence world!.								*/
/* Best class to Cast a MiddleFence type character.											*/
/* Only has necessary components that every characters need, like UTargetHelperComponent.	*/
/********************************************************************************************/

UCLASS()
class MIDDLEFENCE_API AMiddleFenceCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:

	/*Sets default values & components*/
	AMiddleFenceCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());


#pragma region General Methods

protected:

	/** Run once at the start of the play */
	virtual void BeginPlay() override;

#pragma endregion

#pragma region Essential Components

public:

	/**
	 * TargetHelper helps to find the best target.
	 * It is the most necessary component for character
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EssentialComponents")
	class UTargetHelperComponent* TargetHelper;

#pragma endregion

#pragma region Movement Functionality
protected:

	/**
	 * \brief An empty function to implement move forward functionality based on player input.
	 * \brief Override in the child class.
	 * 
	 * @param [const FInputActionValue& Value] Value comes from Enhanced Input.
	 */
	UFUNCTION()
	virtual void MoveForward(const FInputActionValue& Value);

	/**
	 * \brief An empty function to implement move right functionality based on player input.
	 * \brief Override in the child class.
	 * 
	 * @param [const FInputActionValue& Value], Value comes from Enhanced Input.
	 */
	UFUNCTION()
	virtual void MoveRight(const FInputActionValue& Value);


public:

	/********************************************************************************/
	/* Character won't rotate toward anything nor velocity neither controller yaw.	*/
	/********************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void FreeCharacterRotation();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ActiveCharacterStrafeRotation();
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
		void ActiveCharacterVelocityRotation();

#pragma endregion

#pragma region Helper Function

public:

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Helper|Animation")
		UAnimMontage* GetCurrentActiveMontage() const;

	/************************************************************************************************/
	/* Return the time of current position of active animation montage that is playing right now!.  */
	/************************************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Helper|Animation")
	float GetCurrentPlayingMontageTime() const;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Helper|Animation")
	void PauseCurrentActiveAnimMonatge();

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Helper|Animation")
	void PauseAnimMonatge(UAnimMontage* MontageToPause);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Helper|Animation")
	void ResumeAnimMonatge(UAnimMontage* MontageToResume);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Helper|Animation")
	void ResumeCurrentActiveAnimMonatge();

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Helper|Animation")
		void SetPositionCurrentActiveAnimMonatge(float NewPosition);

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Helper|Animation")
		float GetPositionCurrentActiveAnimMonatge() const;
#pragma endregion

};
