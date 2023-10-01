// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/MiddleFenceMainCharacterBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "InputActionValue.h"
#include "Inventory/MiddleFenceInventoryBase.h"
#include "Components/TargetHelperComponent.h"
#include "Game/MiddleFenceGameplayStatics.h"
#include "UserWidget/ChainInputWidget.h"

AMiddleFenceMainCharacterBase::AMiddleFenceMainCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Component"));
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->TargetArmLength = 700.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	//ChainInput = CreateDefaultSubobject<UChainInputComponent>(TEXT("Chain Input Component"));
	//ChainInput->OnAddNewInput.AddUObject(this, &AMiddleFenceMainCharacterBase::OnNewChainInputAddedToBuffer);
}

void AMiddleFenceMainCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (ChainInput)
	{
		//ChainInput->OnUpdateLastInput.AddUObject(this, &AMiddleFenceMainCharacterBase::OnUpdateLastInput);
		ChainInput->OnWaitLongTimeNoInput.AddUObject(this, &AMiddleFenceMainCharacterBase::OnWaitLongTimeNoInput);
		ChainInput->OnCancelInput.AddUObject(this, &AMiddleFenceMainCharacterBase::OnCancelInput);
	}
}



void AMiddleFenceMainCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//PlayerInputComponent->BindAxis("MoveForward", this, &AMiddleFenceMainCharacterBase::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AMiddleFenceMainCharacterBase::MoveRight);

	if (UEnhancedInputComponent* enhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (InputAction_MoveForward)
		{
			enhancedInput->BindAction(InputAction_MoveForward, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::MoveForward);
		}

		if (InputAction_MoveRight)
		{
			enhancedInput->BindAction(InputAction_MoveRight, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::MoveRight);
		}

		if (InputAction_Turn)
		{
			enhancedInput->BindAction(InputAction_Turn, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::OnTurn);
		}

		if (InputAction_LookUp)
		{
			enhancedInput->BindAction(InputAction_LookUp, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::OnLookUp);
		}

		if (InputAction_Shift)
		{
			enhancedInput->BindAction(InputAction_Shift, ETriggerEvent::Started, this, &AMiddleFenceMainCharacterBase::OnShiftKey);
			enhancedInput->BindAction(InputAction_Shift, ETriggerEvent::Completed, this, &AMiddleFenceMainCharacterBase::OnShiftKey);
		}

		if (InputAction_RightWeaponLightAttack)
		{
			enhancedInput->BindAction(InputAction_RightWeaponLightAttack, ETriggerEvent::Started, this, &AMiddleFenceMainCharacterBase::RightWeaponLightAttack);
			enhancedInput->BindAction(InputAction_RightWeaponLightAttack, ETriggerEvent::Completed, this, &AMiddleFenceMainCharacterBase::RightWeaponLightAttackRelease);
		}

		if (InputAction_RightWeaponLightAttack_Hold)
		{
			enhancedInput->BindAction(InputAction_RightWeaponLightAttack_Hold, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::RightWeaponLightAttackHold);
		}

		if (InputAction_RightWeaponHeavyAttack)
		{
			enhancedInput->BindAction(InputAction_RightWeaponHeavyAttack, ETriggerEvent::Started, this, &AMiddleFenceMainCharacterBase::RightWeaponHeavyAttack);
			enhancedInput->BindAction(InputAction_RightWeaponHeavyAttack, ETriggerEvent::Completed, this, &AMiddleFenceMainCharacterBase::RightWeaponHeavyAttackReleased);
		}

		if (InputAction_RightWeaponHeavyAttack_Hold)
		{
			enhancedInput->BindAction(InputAction_RightWeaponHeavyAttack_Hold, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::RightWeaponHeavyAttackHold);
		}

		if (InputAction_LeftWeaponLightAttack)
		{
			enhancedInput->BindAction(InputAction_LeftWeaponLightAttack, ETriggerEvent::Started, this, &AMiddleFenceMainCharacterBase::LeftWeaponLightAttack);
			enhancedInput->BindAction(InputAction_LeftWeaponLightAttack, ETriggerEvent::Completed, this, &AMiddleFenceMainCharacterBase::LeftWeaponLightAttackRelease);
		}

		if (InputAction_LeftWeaponLightAttack_Hold)
		{
			enhancedInput->BindAction(InputAction_LeftWeaponLightAttack_Hold, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::LeftWeaponLightAttackHold);
		}

		if (InputAction_LeftWeaponHeavyAttack)
		{
			enhancedInput->BindAction(InputAction_LeftWeaponHeavyAttack, ETriggerEvent::Started, this, &AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttack);
			enhancedInput->BindAction(InputAction_LeftWeaponHeavyAttack, ETriggerEvent::Completed, this, &AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttackRelease);
		}

		if (InputAction_LeftWeaponHeavyAttack_Hold)
		{
			enhancedInput->BindAction(InputAction_LeftWeaponHeavyAttack_Hold, ETriggerEvent::Triggered, this, &AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttackHold);
		}
	}
}


void AMiddleFenceMainCharacterBase::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* pc = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* inputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			inputSubsystem->ClearAllMappings();
			inputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	SpawnChainInputWidget();
}

void AMiddleFenceMainCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (ChainInputWidget)
	{
		ChainInputWidget->RemoveFromParent();
		ChainInputWidget = nullptr;
	}


}


void AMiddleFenceMainCharacterBase::SpawnChainInputWidget()
{
	if (ChainInputWidget)
	{
		ChainInputWidget->RemoveFromParent();
		ChainInputWidget = nullptr;
	}

	if (ChainInputWidgetTemplate)
	{
		if (ChainInputWidget == nullptr)
		{
			ChainInputWidget = CreateWidget<UChainInputWidget>(GetWorld() ,ChainInputWidgetTemplate);
			if (ChainInputWidget)
			{
				ChainInputWidget->AddToViewport();
			}
		}
	}
}

void AMiddleFenceMainCharacterBase::OnNewChainInputAddedToBuffer(EActionInputType newInput)
{
	if (ChainInputWidget)
	{
		ChainInputWidget->OnAddNewInputToBuffer(newInput);
	}
}

void AMiddleFenceMainCharacterBase::OnWaitLongTimeNoInput()
{
	if (ChainInputWidget)
	{
		ChainInputWidget->OnClearInputBuffer();
	}
}

void AMiddleFenceMainCharacterBase::OnUpdateLastInput(EActionInputType newInput)
{
	if (ChainInputWidget)
	{
		ChainInputWidget->OnUpdateLastInput(newInput);
	}
}

void AMiddleFenceMainCharacterBase::ClearActionInputBuffer()
{
	Super::ClearActionInputBuffer();

	if (ChainInputWidget)
	{
		ChainInputWidget->OnClearInputBuffer();
	}
}

void AMiddleFenceMainCharacterBase::OnCancelInput()
{
	if (GetCurrentActiveWeapon())
	{
		GetCurrentActiveWeapon()->ForceCancel();
	}
}

#pragma region Movement Functionality

void AMiddleFenceMainCharacterBase::MoveForward(const FInputActionValue& Value)
{
	float axisValue = Value.GetMagnitude();

	if (Controller != nullptr && axisValue != 0)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, axisValue);
	}
}

void AMiddleFenceMainCharacterBase::MoveRight(const FInputActionValue& Value)
{
	float axisValue = Value.GetMagnitude();

	if (Controller != nullptr && axisValue != 0)
	{
		const FRotator YawRotation = FRotator(0, Controller->GetControlRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, axisValue);
	}
}

void AMiddleFenceMainCharacterBase::OnTurn(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude());// *GetWorld()->GetDeltaSeconds());
}

void AMiddleFenceMainCharacterBase::OnLookUp(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.GetMagnitude());// *GetWorld()->GetDeltaSeconds());
}

void AMiddleFenceMainCharacterBase::OnShiftKey(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Error, TEXT("OnShiftKey : %f"), Value.GetMagnitude());

	HeavyAttackInputIsActive = Value.GetMagnitude() >= 0.5f ? true : false;
}

#pragma endregion


#pragma region Combat System

void AMiddleFenceMainCharacterBase::RightWeaponLightAttack(const FInputActionValue& Value)
{
	if (HeavyAttackInputIsActive)
		return;

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_RightWeaponLightAttack);
	}
}


void AMiddleFenceMainCharacterBase::RightWeaponLightAttackHold(const FInputActionValue& Value)
{
	if (HeavyAttackInputIsActive)
		return;

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_RightWeaponLightAttackHold);
	}

	//UE_LOG(LogTemp, Error, TEXT("RightWeaponLightAttackHold"));



	//UE_LOG(LogTemp, Error, TEXT("Hold !!!!!!!"));

	//AddNewActionInputToBuffer(EActionInputType::CIT_RightWeaponLightAttackHold);
	//ToggleReceiveActionInput(false);

	//ExecuteActionInput();
}

void AMiddleFenceMainCharacterBase::RightWeaponLightAttackRelease(const FInputActionValue& Value)
{
	if (ChainInput)
	{
		ChainInput->OnInputReleased();
	}
}

void AMiddleFenceMainCharacterBase::RightWeaponHeavyAttack(const FInputActionValue& Value)
{
	if (!HeavyAttackInputIsActive)
		return;

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_RightWeaponHeavyAttack);
	}
}

void AMiddleFenceMainCharacterBase::RightWeaponHeavyAttackHold(const FInputActionValue& Value)
{
	if (!HeavyAttackInputIsActive)
		return;

	//UE_LOG(LogTemp, Error, TEXT("RightWeaponHeavyAttackHold"));

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_RightWeaponHeavyAttackHold);
	}

	//if (PlayerActionInputBuffer.Num() > 0 && PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1].Input == EActionInputType::CIT_RightWeaponHeavyAttackHold)
		//return;

	//AddNewActionInputToBuffer(EActionInputType::CIT_RightWeaponHeavyAttackHold);
	//ToggleReceiveActionInput(false);

	//ExecuteActionInput();
}

void AMiddleFenceMainCharacterBase::RightWeaponHeavyAttackReleased(const FInputActionValue& Value)
{
	if (ChainInput)
	{
		ChainInput->OnInputReleased();
	}
}

void AMiddleFenceMainCharacterBase::LeftWeaponLightAttack(const FInputActionValue& Value)
{
	if (HeavyAttackInputIsActive)
		return;

	//UE_LOG(LogTemp, Error, TEXT("LeftWeaponLightAttack"));

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_LeftWeaponLightAttack);
	}

	//if (!CheckIfActionInputAccepted())
		//return;

	//AddNewActionInputToBuffer(EActionInputType::CIT_LeftWeaponLightAttack);
	//ToggleReceiveActionInput(false);

	//ExecuteActionInput();
}

void AMiddleFenceMainCharacterBase::LeftWeaponLightAttackHold(const FInputActionValue& Value)
{
	if (HeavyAttackInputIsActive)
		return;


	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_LeftWeaponLightAttackHold);
	}
}

void AMiddleFenceMainCharacterBase::LeftWeaponLightAttackRelease(const FInputActionValue& Value)
{
	if (ChainInput)
	{
		ChainInput->OnInputReleased();
	}
}

void AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttack(const FInputActionValue& Value)
{
	if (!HeavyAttackInputIsActive)
		return;


	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_LeftWeaponHeavyAttack);
	}
}

void AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttackHold(const FInputActionValue& Value)
{
	if (!HeavyAttackInputIsActive)
		return;

	if (ChainInput)
	{
		ChainInput->PlayerTryAddNewInput(EActionInputType::CIT_LeftWeaponHeavyAttackHold);
	}
}

void AMiddleFenceMainCharacterBase::LeftWeaponHeavyAttackRelease(const FInputActionValue& Value)
{
	if (ChainInput)
	{
		ChainInput->OnInputReleased();
	}
}

void AMiddleFenceMainCharacterBase::ExecuteActionInput(TArray<FComboInput>InputBuffer)
{
	CheckTarget();

	Super::ExecuteActionInput(InputBuffer);
}

bool AMiddleFenceMainCharacterBase::CheckTarget()
{
	if (!TargetHelper)
		return false;

	/*Fill target if is null*/
	if (TargetHelper->GetCurrentTarget() == nullptr)
	{
		if (TargetHelper->FindAndAssignTarget())
		{
			return true;
		}
	}
	else
	{
		APlayerController* pc = Cast<APlayerController>(GetController());
		if (pc)
		{
			float PlayerIsWatchingAnotherTarget = UMiddleFenceGameplayStatics::GetNegativeAngleBetweenCameraPointAndTarget2D(TargetHelper->GetCurrentTarget(), pc);

			if (PlayerIsWatchingAnotherTarget > 45.0f || PlayerIsWatchingAnotherTarget < -45)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Trying To Find Another Target"));
				if (TargetHelper->FindAndAssignTarget())
				{
					return true;
				}
			}
		}

		//return true for now
		return true;
	}

	return false;
}

#pragma endregion