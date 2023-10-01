// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/MiddleFenceDataTypes.h"
#include "ChainInputComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnAddNewInput, EActionInputType)
DECLARE_MULTICAST_DELEGATE(FDelegateOnWaitLongTimeNoInput)
DECLARE_MULTICAST_DELEGATE(FDelegateOnCancelInput)
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnUpdateLastInput, EActionInputType)
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnExecutionInput, TArray<FComboInput>)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MIDDLEFENCE_API UChainInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UChainInputComponent();

public:

	/************************************************************************/
	/* Get called when a new input added to InputBuffer                     */
	/************************************************************************/
	FDelegateOnAddNewInput OnAddNewInput;


	/************************************************************************/
	/* Get called when a new input didn't add to InputBuffer for long time */
	/************************************************************************/
	FDelegateOnWaitLongTimeNoInput OnWaitLongTimeNoInput;


	/************************************************************************/
	/* Get called when a hold key replaced with normal key.
	/* When a character press a key a new animation start to play(if exist)
	/* so if he hold than key, the hold button replaced with old button and
	/* last animation will cancel and new one will runs. after hold key
	/* accepted then ExecuteInput will run again							*/
	/************************************************************************/
	FDelegateOnCancelInput OnCancelInput;


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//FDelegateOnUpdateLastInput OnUpdateLastInput;


	/************************************************************************/
	/* Get called when ExecuteInput method is ran                           */
	/************************************************************************/
	FDelegateOnExecutionInput OnExecutionInput;

protected:

	/************************************************************************/
	/* Store all accepted inputs from the character                         */
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, Category = "Input|Chain", meta = (ShowOnlyInnerProperties))
		TArray<FComboInput> InputBuffer;

	/************************************************************************/
	/* When the result of chain-input was true so it change to true
	/* When bTriggered is true it means an action in running				*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		bool bTriggered;
 
	/************************************************************************/
	/* It helps to receive new input when bTriggered is true. Actually it
	/* helps to get second input while first input is not consumed yet		*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		bool bNextCaught;

	/************************************************************************/
	/* A helper variable to let us know the current input that the character
	/* try to add is hold key not a normal key								*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
	bool bLastKeyWasHold = false;

	/************************************************************************/
	/* How long should the component wait for a new input to keep the input
	/* connected (chain)													*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, Category = "Input|Chain")
		float WaitTimeToCatchInput;

	/************************************************************************/
	/* A very short delay before executing input at first action. this delay 
	/* is necessary for determining the key is hold or normal				*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, Category = "Input|Chain")
		float FirstExecutationDelay;


	/************************************************************************/
	/* Reasonable time for determining the input was delay					*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, Category = "Input|Chain")
		FVector2D DelayInputAcceptRange;

	/************************************************************************/
	/* Specify when the character can add delay input						*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, Category = "Input|Chain")
		bool bCanAcceptDelay = false;


	FTimerHandle TimerHandle_FirstExecutationWithDelay;
	FTimerHandle TimerHandle_WaitForNextTrigger;
	FTimerHandle TimerHandle_AutoExecuteNext;

protected:

	/************************************************************************/
	/* Only this method allowed to add a new input to InputBuffer           */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Input|Chain")
	void AddNewInputToBuffer(EActionInputType newInputType);


	void ExecuteInput();

	/************************************************************************/
	/* Call ExecuteInput with a short delay                                 */
	/************************************************************************/
	void ExecuteWithDelay();

	/************************************************************************/
	/* If the character added second input while was playing first input
	/* action so as soon as the first input action finish it will run second
	/* input action automatically.											*/
	/************************************************************************/
	void AutoExecuteNext();

	/************************************************************************/
	/* Actually reset the variables, because it did wait so long for new
	/* input and didn't receive yet											*/
	/************************************************************************/
	void OnWaitForNextTrigger();


public:

	/************************************************************************/
	/* A good place to unbind all delegates                                 */
	/************************************************************************/
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/************************************************************************/
	/* Main function for adding input after validation						*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Input|Chain")
	void PlayerTryAddNewInput(EActionInputType newInputType);


	/************************************************************************/
	/* return InputBuffer                                                   */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Input|Chain")
	TArray<FComboInput> GetInputBuffer() const;


protected:

	/************************************************************************/
	/* Checks if the character allowed to add a new input                   */
	/************************************************************************/
	bool AllowedToAddInputToBuffer() const;


	/************************************************************************/
	/* A helper function to check if the newKey is hold key                 */
	/************************************************************************/
	UFUNCTION(BlueprintPure, Category = "Input|Chain")
		bool IsHoldKey(EActionInputType newKey) const;


#pragma region Call from Outside of the component class
public:

	/************************************************************************/
	/* Only clear InputBuffer array. Do not do anything else.				*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Input|Chain")
		void ClearInputBuffer();

	/************************************************************************/
	/* Send the result of chain-input to the component. Checks if the 
	/* chain-input runs an action or not									*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Input|Chain")
		void SendInputExecutionResult(bool ActionIsRunning);

	/************************************************************************/
	/* It is a helper function to let the component know when is a proper
	/* time to accept delay input or when should start the next input
	/* execution timer or when should start timer for checking input delay
	/* to reset.
	/* Probably get called at AnimationPossibleEnd Anim_Notify event through 
	/* AdvanceCharacter class.												*/
	/************************************************************************/
	void OnCurrentActionAlmostFinished();

	/************************************************************************/
	/* Get called at end of current playing animation                       */
	/************************************************************************/
	void OnCurrentActionFinish();
	
	/************************************************************************/
	/* Get called when the character release the key                        */
	/************************************************************************/
	void OnInputReleased();

#pragma endregion

};
