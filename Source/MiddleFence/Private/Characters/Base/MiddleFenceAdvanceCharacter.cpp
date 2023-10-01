// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/MiddleFenceAttributeSetBase.h"
#include "AbilitySystem/MiddleFenceAbilitySysComponent.h"
#include "Characters/MiddleFenceCharMovementComponent.h"
#include "Inventory/MiddleFenceInventoryBase.h"
#include "MotionWarpingComponent.h"
#include "Components/TargetHelperComponent.h"


AMiddleFenceAdvanceCharacter::AMiddleFenceAdvanceCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UMiddleFenceCharMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AbilitySystemComponent = CreateDefaultSubobject<UMiddleFenceAbilitySysComponent>(TEXT("Ability System Component"));

	AttributeSetComponent = CreateDefaultSubobject<UMiddleFenceAttributeSetBase>(TEXT("AttributeSet Component"));
	AttributeSetComponent->OnHealthChange.AddDynamic(this, &AMiddleFenceAdvanceCharacter::OnHealthChange);

	if(GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed = 500;


	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warping Component"));

	bIsDead = false;

	//ActionInputClearTime = 1.0f;
	//DelayInputAcceptRange = FVector2D(1.0f, 1.5f);
}

void AMiddleFenceAdvanceCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//UE_LOG(LogTemp, Warning, TEXT("EndPlay!!!!"));
	if (AttributeSetComponent)
	{
		AttributeSetComponent->OnHealthChange.RemoveDynamic(this, &AMiddleFenceAdvanceCharacter::OnHealthChange);
	}

	OnActionAnimationEnds.RemoveAll(this);
	OnActionAnimationPossibleEnds.RemoveAll(this);
	//OnActionInputEnabled.RemoveAll(this);
}

void AMiddleFenceAdvanceCharacter::UpdateMotionWarpingOnTarget(FName AnimationWarpTargetName, float PositionOffset)
{
	if (AnimationWarpTargetName.IsNone() == false)
	{
		FVector WarpTargetPos = GetActorLocation();
		FRotator WarpTargetRot = GetActorRotation();

		if (TargetHelper->GetCurrentTarget())
		{
			FVector TargetLoc = TargetHelper->GetCurrentTarget()->GetActorLocation();
			FVector Direction = TargetLoc - WarpTargetPos;
			Direction.Normalize();

			float Offset = PositionOffset;
			WarpTargetPos = TargetLoc - (Direction * Offset);
			WarpTargetRot = Direction.Rotation();
			GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(AnimationWarpTargetName, WarpTargetPos, WarpTargetRot);
		}
		else
		{
			GetMotionWarpingComponent()->RemoveWarpTarget(AnimationWarpTargetName);
		}
	}
}

//void AMiddleFenceAdvanceCharacter::PostInitializeComponents()
//{
//	Super::PostInitializeComponents();
//	
//
//}

void AMiddleFenceAdvanceCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

void AMiddleFenceAdvanceCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	if (ChainInput)
	{
		ChainInput->OnAddNewInput.AddUObject(this, &AMiddleFenceAdvanceCharacter::OnNewChainInputAddedToBuffer);
		ChainInput->OnExecutionInput.AddUObject(this, &AMiddleFenceAdvanceCharacter::ExecuteActionInput);
	}

	SpawnInventoryDefaultItems();

}

void AMiddleFenceAdvanceCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);


	GiveAbilities();
	ApplyStartupEffect();
}

//void AMiddleFenceAdvanceCharacter::OnRep_PlayerState()
//{
//	Super::OnRep_PlayerState();
//	AbilitySystemComponent->InitAbilityActorInfo(this, this);
//}

#pragma region Ability System

UAbilitySystemComponent* AMiddleFenceAdvanceCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAbilitySystemComponent* AMiddleFenceAdvanceCharacter::GetMyAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMiddleFenceAdvanceCharacter::GiveAbilities()
{
	if (AbilitySystemComponent)
	{
		if(CharacterData.DefaultAbilities.Num() > 0)
		{
			for (auto ability : CharacterData.DefaultAbilities)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(ability));
			}
		}
	}
}

void AMiddleFenceAdvanceCharacter::ApplyStartupEffect()
{
	if (AbilitySystemComponent)
	{
		for (auto effect : CharacterData.DefaultEffects)
		{
			ApplyGameplayEffectToSelf(effect);
		}
	}
}

void AMiddleFenceAdvanceCharacter::SetCharacterData(const FCharacterData& data)
{
	CharacterData = data;

}

FCharacterData AMiddleFenceAdvanceCharacter::GetCharacterData() const
{
	return CharacterData;
}

bool AMiddleFenceAdvanceCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect)
{
	if (!Effect.Get() || !AbilitySystemComponent)
		return false;

	FGameplayEffectContextHandle GEContextHandle = AbilitySystemComponent->MakeEffectContext();
	GEContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, GEContextHandle);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}




#pragma endregion

#pragma region Action Input

void AMiddleFenceAdvanceCharacter::OnNewChainInputAddedToBuffer(EActionInputType newInput)
{

}

//void AMiddleFenceAdvanceCharacter::AddNewActionInputToBuffer(EActionInputType newInputType)
//{
//	if (UWorld* world = GetWorld())
//	{
//		FComboInput input = FComboInput();
//		input.Input = newInputType;
//		input.AddedTime = world->GetTimeSeconds();
//
//		//UE_LOG(LogTemp, Display, TEXT("AddNewActionInputToBuffer added time : %f"), input.AddedTime);
//
//		/*Clear buffer if it took so long time to add a new input*/
//		if (PlayerActionInputBuffer.Num() > 0)
//		{
//			FComboInput lastInput = PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1];
//
//			float deltaTime = world->GetTimeSeconds() - lastInput.AddedTime;
//			UE_LOG(LogTemp, Display, TEXT("deltaTime : %f"), deltaTime);
//
//			if (deltaTime > ActionInputClearTime)
//			{
//				PlayerActionInputBuffer.Empty();
//			}
//			else
//			{
//				if (deltaTime >= DelayInputAcceptRange.X && deltaTime < DelayInputAcceptRange.Y)
//				{
//					FComboInput delayInput = FComboInput();
//					delayInput.Input = EActionInputType::CIT_Delay;
//					delayInput.AddedTime = world->GetTimeSeconds();
//
//					PlayerActionInputBuffer.Add(delayInput);
//				}
//			}
//
//			if (newInputType == EActionInputType::CIT_RightWeaponLightAttackHold)
//			{
//				//if(PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1].Input == EActionInputType::CIT_RightWeaponLightAttack)
//				PlayerActionInputBuffer.RemoveAt(PlayerActionInputBuffer.Num() - 1);
//			}
//		}
//
//		PlayerActionInputBuffer.Add(input);
//
//		if (InventoryItemIsRunning)
//		{
//			NewActionInputReceivedWhenInventoryItemWasRunning = true;
//		}
//	}
//}

//bool AMiddleFenceAdvanceCharacter::CheckIfActionInputAccepted() const
//{
//	if (NewActionInputReceivedWhenInventoryItemWasRunning)
//		return false;
//
//	if (bActionInputIsMatch)
//	{
//		return bCanReceiveActionInput;
//	}
//	else
//	{
//		return !InventoryItemIsRunning;
//	}
//
//	return true;
//}
//
//TArray<FComboInput> AMiddleFenceAdvanceCharacter::GetPlayerActionInputBuffer() const
//{
//	return PlayerActionInputBuffer;
//}

void AMiddleFenceAdvanceCharacter::ClearActionInputBuffer()
{
	//PlayerActionInputBuffer.Empty();

	if (ChainInput)
	{
		ChainInput->ClearInputBuffer();
	}
}

//void AMiddleFenceAdvanceCharacter::ToggleReceiveActionInput(bool canReceive)
//{
//	bCanReceiveActionInput = canReceive;
//}


//void AMiddleFenceAdvanceCharacter::UpdateLastActionInputTimeOnAnimationPossibleEnd()
//{
//	if (UWorld* world = GetWorld())
//	{
//		if (PlayerActionInputBuffer.Num() > 0)
//		{
//			FComboInput lastInput = PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1];
//
//			float oldTime = PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1].AddedTime;
//			float newTime = world->GetTimeSeconds();
//
//			PlayerActionInputBuffer[PlayerActionInputBuffer.Num() - 1].AddedTime = newTime;
//
//		}
//	}
//}

//void AMiddleFenceAdvanceCharacter::ResetActionInputOnDelay()
//{
//	PlayerActionInputBuffer.Empty();
//	NewActionInputReceivedWhenInventoryItemWasRunning = false;
//	bActionInputIsMatch = false;
//	bCanReceiveActionInput = true;
//
//	//UE_LOG(LogTemp, Display, TEXT("ResetActionInputOnDelay !!!"));
//}

void AMiddleFenceAdvanceCharacter::ExecuteActionInput(TArray<FComboInput>InputBuffer)
{
	//if (PlayerActionInputBuffer.Num() == 0)
		//return;

	//bActionInputIsMatch = false;

	bool aWeaponRuns = false;

	for (FInventorySlot inventory : InventoryItemsSlots)
	{
		if (inventory.ItemInstance)
		{
			AMiddleFenceInventoryBase* activeWeapon = inventory.ItemInstance->OnAction(ChainInput->GetInputBuffer());
			if (activeWeapon)
			{
				aWeaponRuns = true;
				if (activeWeapon != CurrentActiveWeapon)
				{
					LastActiveWeapon = CurrentActiveWeapon;
					SwitchWeapon(activeWeapon, CurrentActiveWeapon);
				}

				CurrentActiveWeapon = activeWeapon;


				if (ChainInput)
				{
					ChainInput->SendInputExecutionResult(true);
				}
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("aWeaponRuns %i"), aWeaponRuns);

	if (aWeaponRuns == false)
	{
		if (ChainInput)
		{
			ChainInput->SendInputExecutionResult(false);
		}
	}
}

#pragma endregion

#pragma region Health & Hit

void AMiddleFenceAdvanceCharacter::OnHealthChange_Implementation(float Health, float MaxHealth, AActor*& whoSendDamage)
{
	UE_LOG(LogTemp, Warning, TEXT("On Health Change : %f, Target : %s"), Health, *GetNameSafe(whoSendDamage));

	if (bIsDead == false)
	{
		if (Health <= 0)
		{
			bIsDead = true;

			if (whoSendDamage)
			{
				AMiddleFenceAdvanceCharacter* advanceCharacter = Cast< AMiddleFenceAdvanceCharacter>(whoSendDamage);
				if (advanceCharacter)
				{
					advanceCharacter->OnKillEnemy(this);
				}
			}

			OnDeath();
			return;
		}
		else
		{

			PlayAnimMontage(TestHitAnim);
		}
	}

}

void AMiddleFenceAdvanceCharacter::OnDeath_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	StopAnimMontage();
	PlayAnimMontage(TestDeathAnim);
}

bool AMiddleFenceAdvanceCharacter::IsDead() const
{
	return bIsDead;
}

void AMiddleFenceAdvanceCharacter::OnKillEnemy(AActor* KilledActor)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKillEnemy"));

	if (KilledActor)
	{
		if (TargetHelper && TargetHelper->GetCurrentTarget() && TargetHelper->GetCurrentTarget() == KilledActor)
		{
			TargetHelper->ClearCurrentTarget();
		}
	}
}

#pragma endregion

#pragma region Inventory


void AMiddleFenceAdvanceCharacter::SpawnInventoryDefaultItems()
{
	for (TSubclassOf<AMiddleFenceInventoryBase> item : InventoryDefaultItems)
	{
		AMiddleFenceInventoryBase* newItem = GetWorld()->SpawnActorDeferred<AMiddleFenceInventoryBase>(item, FTransform::Identity, this, this);
		newItem->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, newItem->CharacterAttachPointSocket);

		newItem->FinishSpawning(FTransform::Identity);

		newItem->OnAfterSpawning();

		//AddNewItemToInventoryList(newItem);

		if (AbilitySystemComponent && newItem->GetItemAbilityClass())
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(newItem->GetItemAbilityClass()));
		}

		/*Add new Slot to inventory slots*/
		FInventorySlot slot = FInventorySlot();
		slot.ItemInstance = newItem;
		slot.ItemType = EInventorySlotType::None;

		if (newItem->GetInventoryItemType() == EInventoryItemType::Weapon)
		{
			if (IsRightWeaponInventorySlotEmpty())
			{
				slot.ItemType = EInventorySlotType::RightWeapon;
				newItem->UpdateInventorySlotType(EInventorySlotType::RightWeapon);
				
			}
			else if (IsLeftWeaponInventorySlotEmpty())
			{
				slot.ItemType = EInventorySlotType::LeftWeapon;
				newItem->UpdateInventorySlotType(EInventorySlotType::LeftWeapon);
			}
		}

		AddNewItemToInventorySlot(slot);
	}


	//init default weapon
	bool activeWeaponIsSet = false;
	for (FInventorySlot slot : InventoryItemsSlots)
	{
		if (slot.ItemInstance)
		{
			slot.ItemInstance->SetActorHiddenInGame(true);

			if (!activeWeaponIsSet && (slot.ItemType == EInventorySlotType::RightWeapon || slot.ItemType == EInventorySlotType::LeftWeapon))
			{
				activeWeaponIsSet = true;
				slot.ItemInstance->SetActorHiddenInGame(false);
				CurrentActiveWeapon = slot.ItemInstance;
			}
		}
	}
}

bool AMiddleFenceAdvanceCharacter::IsRightWeaponInventorySlotEmpty() const
{
	if (InventoryItemsSlots.Num() == 0)
		return true;

	for (FInventorySlot invSlot : InventoryItemsSlots)
	{
		if (invSlot.ItemType == EInventorySlotType::RightWeapon && invSlot.ItemInstance)
		{
			return false;
			break;
		}
	}

	return true;
}

bool AMiddleFenceAdvanceCharacter::IsLeftWeaponInventorySlotEmpty() const
{
	if (InventoryItemsSlots.Num() == 0)
		return true;

	for (FInventorySlot invSlot : InventoryItemsSlots)
	{
		if (invSlot.ItemType == EInventorySlotType::LeftWeapon && invSlot.ItemInstance)
		{
			return false;
			break;
		}
	}

	return true;
}

AMiddleFenceInventoryBase* AMiddleFenceAdvanceCharacter::GetCurrentActiveWeapon() const
{
	return CurrentActiveWeapon;
}

void AMiddleFenceAdvanceCharacter::AddNewItemToInventorySlot(FInventorySlot newItemSlot)
{
	InventoryItemsSlots.Add(newItemSlot);
}

TArray<FInventorySlot> AMiddleFenceAdvanceCharacter::GetInventoryItemSlots() const
{
	return InventoryItemsSlots;
}

void AMiddleFenceAdvanceCharacter::SwitchWeapon(AMiddleFenceInventoryBase* newWeapon, AMiddleFenceInventoryBase* oldWeapon)
{
	if (newWeapon)
	{
		newWeapon->SetActorHiddenInGame(false);
		newWeapon->SetActorEnableCollision(true);
	}


	if (oldWeapon)
	{
		oldWeapon->SetActorHiddenInGame(true);
		newWeapon->SetActorEnableCollision(false);
	}
}

void AMiddleFenceAdvanceCharacter::ForceCancelActiveWeapon()
{
	if (CurrentActiveWeapon)
	{
		CurrentActiveWeapon->ForceCancel();
	}
}

void AMiddleFenceAdvanceCharacter::OnActionAnimationStart()
{
}

void AMiddleFenceAdvanceCharacter::OnActionAnimationPossibleEnd()
{
	OnActionAnimationPossibleEnds.Broadcast();

	if (ChainInput)
	{
		ChainInput->OnCurrentActionAlmostFinished();
	}

}

void AMiddleFenceAdvanceCharacter::OnActionAnimationEnd()
{
	OnActionAnimationEnds.Broadcast();

	if (ChainInput)
	{
		ChainInput->OnCurrentActionFinish();
	}
}

//void AMiddleFenceAdvanceCharacter::OnActionAnimationCanReceiveInput()
//{
	//OnActionInputEnabled.Broadcast();
//}

#pragma endregion
