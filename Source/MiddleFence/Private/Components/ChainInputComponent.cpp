// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ChainInputComponent.h"


UChainInputComponent::UChainInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;

	WaitTimeToCatchInput = 2.0f;
	FirstExecutationDelay = 0.15f;
	DelayInputAcceptRange = FVector2D(0.75f, 1.25f);
}


void UChainInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnAddNewInput.RemoveAll(this);
	OnWaitLongTimeNoInput.RemoveAll(this);
	OnExecutionInput.RemoveAll(this);
	//OnUpdateLastInput.RemoveAll(this);
	OnCancelInput.RemoveAll(this);
}

void UChainInputComponent::AddNewInputToBuffer(EActionInputType newInputType)
{
	if (UWorld* world = GetWorld())
	{
		FComboInput input = FComboInput();
		input.Input = newInputType;
		input.AddedTime = world->GetTimeSeconds();


		InputBuffer.Add(input);
		OnAddNewInput.Broadcast(newInputType);
	}
}

bool UChainInputComponent::AllowedToAddInputToBuffer() const
{
	if (bLastKeyWasHold)
		return true;

	if (bTriggered)
		return !bNextCaught;

	return !bTriggered;
}

void UChainInputComponent::PlayerTryAddNewInput(EActionInputType newInputType)
{
	if (bLastKeyWasHold)
		return;

	

	if (bLastKeyWasHold == false && IsHoldKey(newInputType))
	{
		if (InputBuffer.Num() > 0 && InputBuffer[InputBuffer.Num() - 1].Input != newInputType)
		{
			InputBuffer.RemoveAt(InputBuffer.Num() - 1);

			OnCancelInput.Broadcast();

			if (TimerHandle_AutoExecuteNext.IsValid() == false)
			{
				if (UWorld* world = GetWorld())
				{
					world->GetTimerManager().SetTimer(TimerHandle_AutoExecuteNext, this, &UChainInputComponent::AutoExecuteNext, 0.016f, true);
				}
			}

			if (TimerHandle_WaitForNextTrigger.IsValid() == false)
			{
				if (UWorld* world = GetWorld())
				{
					world->GetTimerManager().SetTimer(TimerHandle_WaitForNextTrigger, this, &UChainInputComponent::OnWaitForNextTrigger, WaitTimeToCatchInput, false);
				}
			}
		}

		bLastKeyWasHold = true;
	}

	if (!AllowedToAddInputToBuffer())
		return;

	if (bCanAcceptDelay && !IsHoldKey(newInputType) && InputBuffer.Num() > 0)
	{
		if (UWorld* world = GetWorld())
		{
			FComboInput lastInput = InputBuffer[InputBuffer.Num() - 1];
			float deltaTime = world->GetTimeSeconds() - lastInput.AddedTime;

			//UE_LOG(LogTemp, Error, TEXT("deltaTime : %f"), deltaTime);
			if (deltaTime >= DelayInputAcceptRange.X && deltaTime < DelayInputAcceptRange.Y)
			{
				AddNewInputToBuffer(EActionInputType::CIT_Delay);
			}
		}
	}


	//{
	//	FString NewTypeString = "";
	//	if (newInputType == EActionInputType::CIT_RightWeaponLightAttack)
	//		NewTypeString = "CIT_RightWeaponLightAttack";
	//	else if (newInputType == EActionInputType::CIT_RightWeaponLightAttackHold)
	//		NewTypeString = "CIT_RightWeaponLightAttackHold";
	//	else NewTypeString = "Unknown";

	//	UE_LOG(LogTemp, Error, TEXT("newInputType %s"), *NewTypeString);
	//}

	AddNewInputToBuffer(newInputType);


	if (bTriggered)
	{
		bNextCaught = true;
	}
	else
	{
		if (TimerHandle_WaitForNextTrigger.IsValid())
		{
			if (UWorld* world = GetWorld())
			{
				world->GetTimerManager().ClearTimer(TimerHandle_WaitForNextTrigger);
			}
		}

		if (InputBuffer.Num() == 1)
		{
			if (!TimerHandle_FirstExecutationWithDelay.IsValid())
			{
				if (UWorld* world = GetWorld())
				{
					world->GetTimerManager().SetTimer(TimerHandle_FirstExecutationWithDelay, this, &UChainInputComponent::ExecuteWithDelay, FirstExecutationDelay, false);
				}
			}
		}
		else
		{
			ExecuteInput();
		}
	}

}

TArray<FComboInput> UChainInputComponent::GetInputBuffer() const
{
	return InputBuffer;
}

void UChainInputComponent::ExecuteInput()
{
	if (InputBuffer.Num() == 0)
		return;

	bCanAcceptDelay = false;
	OnExecutionInput.Broadcast(InputBuffer);
}

void UChainInputComponent::ExecuteWithDelay()
{
	ExecuteInput();

	if (TimerHandle_FirstExecutationWithDelay.IsValid())
	{
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().ClearTimer(TimerHandle_FirstExecutationWithDelay);
		}
	}
}

void UChainInputComponent::OnWaitForNextTrigger()
{
	InputBuffer.Empty();

	bTriggered = false;
	bNextCaught = false;
	bCanAcceptDelay = false;

	if (TimerHandle_AutoExecuteNext.IsValid())
	{
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().ClearTimer(TimerHandle_AutoExecuteNext);
		}
	}
	
	OnWaitLongTimeNoInput.Broadcast();
}

void UChainInputComponent::AutoExecuteNext()
{
	bool bClearTimer = false;

	if (bNextCaught)
	{
		bNextCaught = false;

		ExecuteInput();

		bClearTimer = true;
	}

	if (InputBuffer.Num() == 0)
	{
		bClearTimer = true;
		bTriggered = false;
		bCanAcceptDelay = false;
	}


	if (bClearTimer)
	{
		if (TimerHandle_AutoExecuteNext.IsValid())
		{
			if (UWorld* world = GetWorld())
			{
				world->GetTimerManager().ClearTimer(TimerHandle_AutoExecuteNext);
			}
		}
	}

	//UE_LOG(LogTemp, Error, TEXT("Check.. %i"), bClearTimer);
}

bool UChainInputComponent::IsHoldKey(EActionInputType newKey) const
{
	return newKey == EActionInputType::CIT_LeftWeaponHeavyAttackHold || newKey == EActionInputType::CIT_LeftWeaponLightAttackHold || newKey == EActionInputType::CIT_RightWeaponHeavyAttackHold || newKey == EActionInputType::CIT_RightWeaponLightAttackHold;
}


#pragma region Call from Outside of the component class

void UChainInputComponent::ClearInputBuffer()
{
	InputBuffer.Empty();
}

void UChainInputComponent::SendInputExecutionResult(bool actionIsRunning)
{
	bTriggered = actionIsRunning;

	if (bTriggered)
	{
		if (TimerHandle_WaitForNextTrigger.IsValid())
		{
			if (UWorld* world = GetWorld())
			{
				world->GetTimerManager().ClearTimer(TimerHandle_WaitForNextTrigger);
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("actionIsRunning : %i"), actionIsRunning);

	if (actionIsRunning == false)
	{
		OnWaitForNextTrigger();
	}
}

void UChainInputComponent::OnCurrentActionAlmostFinished()
{
	bCanAcceptDelay = true;

	if (TimerHandle_AutoExecuteNext.IsValid() == false)
	{
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().SetTimer(TimerHandle_AutoExecuteNext, this, &UChainInputComponent::AutoExecuteNext, 0.016, true);
		}
	}

	if (TimerHandle_WaitForNextTrigger.IsValid() == false)
	{
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().SetTimer(TimerHandle_WaitForNextTrigger, this, &UChainInputComponent::OnWaitForNextTrigger, WaitTimeToCatchInput, false);
		}
	}
}

void UChainInputComponent::OnCurrentActionFinish()
{
}

void UChainInputComponent::OnInputReleased()
{
	bLastKeyWasHold = false;
}

#pragma endregion

