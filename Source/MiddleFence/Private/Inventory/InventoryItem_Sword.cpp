// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem_Sword.h"
#include "Characters/Base/MiddleFenceAdvanceCharacter.h"
#include "Game/MiddleFenceGameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <Engine/Private/KismetTraceUtils.h>
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/Base/MiddleFenceMainCharacterBase.h"

AInventoryItem_Sword::AInventoryItem_Sword()
{
	SwordMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword Mesh Component"));
	SwordMeshComponent->SetupAttachment(RootComponent);

	TraceRadius = 25;
	TraceLength = 150;
	NextTraceTime = 0.1f;
}

void AInventoryItem_Sword::OnAfterSpawning()
{
	if (AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner()) )
	{
		DelegateHandle_OnAnimationPossibleEnd = advanceCharacter->OnActionAnimationPossibleEnds.AddUObject(this, &AInventoryItem_Sword::OnAnimationPossibleEnd);
		//advanceCharacter->OnActionInputEnabled.AddUObject(this, &AInventoryItem_Sword::OnActionInputActivation);
	}

	Super::OnAfterSpawning();

	if (ComboAsset)
	{
		for (UDataTable* table : ComboAsset->ComboDataTables)
		{
			for (auto it : table->GetRowMap())
			{
				FSwordTraceAnimationDataRow* row = reinterpret_cast<FSwordTraceAnimationDataRow*>(it.Value);
				ComboData.Add(*row);
			}
		}
	}
}

AMiddleFenceInventoryBase* AInventoryItem_Sword::OnAction(TArray<FComboInput>InputBuffer)
{
	if (!GetOwner())
		return nullptr;

	if (InputBuffer.Num() == 0)
		return nullptr;

	UAbilitySystemComponent* abilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (abilityComponent)
	{
		if (abilityComponent->HasMatchingGameplayTag(OnSwordPlayingTag))
		{
			//bNewActionInputAdded = true;
			return nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Ability System Component is null. Owner is %s, actor is %s"), *GetNameSafe(GetOwner()), *GetNameSafe(this));
		return nullptr;
	}


	bool ComboRun = ManageComboInput(InputBuffer);

	if (!ComboRun)
		return nullptr;


	AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (advanceCharacter)
	{
		advanceCharacter->FreeCharacterRotation();
	}


	FGameplayEventData payload;
	payload.Instigator = this;
	payload.EventTag = AbilityEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), AbilityEventTag, payload);

	OnSwordAnimationStart();

	return this;
}

void AInventoryItem_Sword::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner()))
	{
		advanceCharacter->OnActionAnimationPossibleEnds.Remove(DelegateHandle_OnAnimationPossibleEnd);
	}
}

void AInventoryItem_Sword::InitializeSwordData(FSwordTraceAnimationDataRow comboData)
{
	CurrentSwordTraceData = comboData;

	TraceRadius = CurrentSwordTraceData.TraceRadius;
	TraceLength = CurrentSwordTraceData.TraceSize;
	TraceTimeSampling = CurrentSwordTraceData.TraceSamplingTime;
	HitPower = CurrentSwordTraceData.HitPower;
	FramePauseTime = CurrentSwordTraceData.FramePauseTimeOnTrace;

	if (CurrentSwordTraceData.Shake_Override)
	{
		CurrentShake = CurrentSwordTraceData.Shake_Override;
	}
	else
	{
		CurrentShake = DefaultShake;
	}

	if (CurrentSwordTraceData.bForceEnableTraceDebug)
	{
		bDrawEachAnimationTraceDebug = true;
	}
}


bool AInventoryItem_Sword::ManageComboInput(TArray<FComboInput>InputBuffer)
{
	AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (advanceCharacter)
	{
		if (InputBuffer.Num() == 0)
			return false;
			
		FComboChain chainInput = FComboChain();
		chainInput.CharacterInputs = InputBuffer;
		chainInput.SlotType = InventorySlotType;


		for (FSwordTraceAnimationDataRow cData : ComboData)
		{
			if (chainInput.Matches(cData.ActionInputs))
			{
				UE_LOG(LogTemp, Display, TEXT("combo name : %s"), *cData.ActionName.ToString());
				InitializeSwordData(cData);

				if (cData.ClearActionInputBuffer)
				{
					advanceCharacter->ClearActionInputBuffer();
				}

				return true;
				break;
			}
		}

	}

	return false;
}


void AInventoryItem_Sword::ClearExcludeTraceActors()
{
	ExcludeTraceActors.Empty();
}

UAnimMontage* AInventoryItem_Sword::GetCurrentComboAnimMontage() const
{
	return CurrentSwordTraceData.Animation;
}

float AInventoryItem_Sword::GetWarpMotionDistanceFromTarget() const
{
	return CurrentSwordTraceData.MotionWarpingDistanceToTarget;
}

void AInventoryItem_Sword::TryTrace()
{
	if (bIsTracing)
		return;

	AMiddleFenceAdvanceCharacter* playerCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());

	if (!playerCharacter)
		return;

	USkeletalMeshComponent* PlayerMesh = playerCharacter->GetMesh();
	if (!PlayerMesh)
		return;

	if (PlayerMesh->GetBoneIndex(CharacterHandBoneForTrace) == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("%s, %s- CharacterHandBoneName Is None, Try set a valid name"), *GetNameSafe(this), *GetNameSafe(GetOwner()));
		return;
	}

	ExcludeTraceActors.Empty();
	
	SwordTraceCounter = 0;
	//SwordTraceLength = TraceTimeSampling.Num();
	NextTraceTime = 0.01f;

	TimerDelegate_SingleTrace.BindUObject(this, &AInventoryItem_Sword::DoSingleTrace, PlayerMesh);

	if (UWorld* world = GetWorld())
	{
		bIsTracing = true;
		world->GetTimerManager().SetTimer(TimerHandle_SingleTrace, TimerDelegate_SingleTrace, NextTraceTime, true);
	}

}

void AInventoryItem_Sword::DoSingleTrace(USkeletalMeshComponent* PlayerMesh)
{
	//&& SwordTraceCounter < SwordTraceLength
	if (TraceTimeSampling.Num() > SwordTraceCounter && PlayerMesh->GetAnimInstance() && PlayerMesh->GetAnimInstance()->GetCurrentActiveMontage())
	{
		float animTime = TraceTimeSampling[SwordTraceCounter];

		float NextIndexTimeOffset = 0.0f;
		int NextIndex = SwordTraceCounter + 1;
		if (NextIndex >= TraceTimeSampling.Num())
		{
			NextIndex = TraceTimeSampling.Num() - 1;
			NextIndexTimeOffset += 0.1f;
		}

		float nextAnimTime = TraceTimeSampling[NextIndex] + NextIndexTimeOffset;

		
		NextTraceTime = nextAnimTime - animTime;
		NextTraceTime /= PlayerMesh->GetAnimInstance()->GetCurrentActiveMontage()->RateScale;

		if (NextTraceTime <= 0)
			NextTraceTime = 0.01f;

		//UE_LOG(LogTemp, Warning, TEXT("NextTraceTime : %f"), NextTraceTime);
		//UE_LOG(LogTemp, Error, TEXT("NextTraceTime : %f, nextAnimTime : %f, animTime : %f"), NextTraceTime, nextAnimTime, animTime);


		FTransform BoneTransform = UMiddleFenceGameplayStatics::GetBoneTransformFromMontage(PlayerMesh, PlayerMesh->GetAnimInstance()->GetCurrentActiveMontage(), CharacterHandBoneForTrace, animTime);

		bool bSwordHitWasSuccess = false;

		if (UWorld* world = GetWorld())
		{
			FVector TraceStartPos = BoneTransform.GetLocation();
			FVector TraceEndPos = TraceStartPos + (BoneTransform.GetRotation().Vector() * -TraceLength);
			
			FCollisionObjectQueryParams ObjectParams; // = ConfigureCollisionObjectParams(TraceObjectTypes);
			for (ECollisionChannel channel : TraceObjectTypes)
			{
				ObjectParams.AddObjectTypesToQuery(channel);
			}


			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());
			QueryParams.AddIgnoredActor(this);

			//debug
			if(bDrawTrace || bDrawEachAnimationTraceDebug)
				DrawDebugCylinder(world, TraceStartPos, TraceEndPos, TraceRadius, 5, TraceDrawColor, false, DrawliveTime);

			TArray<FHitResult> HitInfos = TArray<FHitResult>();


			if (world->SweepMultiByObjectType(HitInfos, TraceStartPos, TraceEndPos, FQuat::Identity, ObjectParams, FCollisionShape::MakeSphere(TraceRadius), QueryParams))
			{
				if (HitInfos.Num() > 0)
				{
					for (FHitResult traceResult : HitInfos)
					{

						if (traceResult.GetActor() && traceResult.GetComponent() )
						{
							bool bApplied = false;
							for (AActor* excludeActor : ExcludeTraceActors)
							{
								if (excludeActor == traceResult.GetActor())
								{
									bApplied = true;
									break;
								}
							}

							if (!bApplied)
							{
								ExcludeTraceActors.AddUnique(traceResult.GetActor());

								/*Fill event data*/
								FGameplayEventData eventData;
								eventData.Target = traceResult.GetActor();

								/*Custom data, it is test for now*/
								USwordHitGameplayEventData* traceHitData = NewObject<USwordHitGameplayEventData>();
								traceHitData->SetSwordHitGameplayEventData(traceResult.ImpactPoint, traceResult.ImpactNormal);
								eventData.OptionalObject = traceHitData;

								/*Send Event*/
								UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), SendEventTag_OnTrace, eventData);

								/*Hit*/
								OnSwordHit(traceResult.GetActor(), traceResult.GetComponent(), traceResult.ImpactPoint, traceResult.ImpactNormal);


								AMiddleFenceAdvanceCharacter* playerCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
								if (playerCharacter)
								{
									playerCharacter->PauseCurrentActiveAnimMonatge();
									float AnimTimeOffset = playerCharacter->GetPositionCurrentActiveAnimMonatge() - nextAnimTime;
									AnimTimeOffset /= 2.0f;
									playerCharacter->SetPositionCurrentActiveAnimMonatge(nextAnimTime + AnimTimeOffset);

									world->GetTimerManager().SetTimer(TimerHandle_ResumeMontage, this, &AInventoryItem_Sword::ResumeMontage, FramePauseTime, false);

									//Clear SingleTrace Timer and continue after resuming animation
									world->GetTimerManager().ClearTimer(TimerHandle_SingleTrace);

									//hit with delay
									TimerDelegate_SendHitDataAfterResume.BindUObject(this, &AInventoryItem_Sword::OnSwordHitAfterResume, traceResult.GetActor(), traceResult.GetComponent());
									world->GetTimerManager().SetTimer(TimerHandle_SendHitDataAfterResume, TimerDelegate_SendHitDataAfterResume, FramePauseTime, false);
								}

								bSwordHitWasSuccess = true;
							}
						}
						
					}
				}
			}
		}

		SwordTraceCounter++;

		//reset timer with new trace time
		if (!bSwordHitWasSuccess)
		{
			if (UWorld* world = GetWorld())
			{
				world->GetTimerManager().ClearTimer(TimerHandle_SingleTrace);
				world->GetTimerManager().SetTimer(TimerHandle_SingleTrace, TimerDelegate_SingleTrace, NextTraceTime, true);
			}
		}
	}
	else
	{
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().ClearTimer(TimerHandle_SingleTrace);
			bIsTracing = false;
		}
	}

}

void AInventoryItem_Sword::ResumeMontage()
{
	AMiddleFenceAdvanceCharacter* playerCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (playerCharacter)
	{
		playerCharacter->ResumeCurrentActiveAnimMonatge();
	}

	if (UWorld* world = GetWorld())
	{
		//Clear pause anim montage timer ... not necessary
		world->GetTimerManager().ClearTimer(TimerHandle_ResumeMontage);

		//Continue Single Trace timer
		world->GetTimerManager().SetTimer(TimerHandle_SingleTrace, TimerDelegate_SingleTrace, NextTraceTime, true);
	}
}

void AInventoryItem_Sword::OnSwordAnimationStart()
{
	AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (advanceCharacter)
	{
		advanceCharacter->OnActionAnimationStart();
	}
}

void AInventoryItem_Sword::OnSwordAnimationEnd()
{
	AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (advanceCharacter)
	{
		//advanceCharacter->ActiveCharacterStrafeRotation();
		advanceCharacter->ActiveCharacterVelocityRotation();
		advanceCharacter->OnActionAnimationEnd();
	}

	bDrawEachAnimationTraceDebug = false;

	//AMiddleFenceMainCharacterBase* mainCharacter = Cast<AMiddleFenceMainCharacterBase>(GetOwner());
	//if (mainCharacter)
	//{
	//	mainCharacter->ClearActionInputOnDelay();
	//}
}

void AInventoryItem_Sword::OnAnimationPossibleEnd()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnAnimationPossibleEnd"));
	UAbilitySystemComponent* abilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	if (abilityComponent)
	{
		if (abilityComponent->HasMatchingGameplayTag(OnSwordPlayingTag))
		{
			abilityComponent->RemoveLooseGameplayTag(OnSwordPlayingTag);
		}
	}

	//if (bNewActionInputAdded)
	//{
	//	OnAction();
	//	bNewActionInputAdded = false;
	//}
	//AMiddleFenceMainCharacterBase* mainCharacter = Cast<AMiddleFenceMainCharacterBase>(GetOwner());
	//if (mainCharacter)
	//{
	//	mainCharacter->ToggleReceiveActionInput(true);
	//}
}

void AInventoryItem_Sword::ForceCancel()
{
	UE_LOG(LogTemp, Error, TEXT("ForceCancel"));

	OnSwordAnimationEnd();
	OnAnimationPossibleEnd();

	AMiddleFenceAdvanceCharacter* advanceCharacter = Cast<AMiddleFenceAdvanceCharacter>(GetOwner());
	if (advanceCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("CancelAbility 0"));
		if (UAbilitySystemComponent* abilityComponent = advanceCharacter->GetMyAbilitySystemComponent())
		{
			UE_LOG(LogTemp, Error, TEXT("CancelAbility 1"));
			TArray<FGameplayAbilitySpec>& Abilities = abilityComponent->GetActivatableAbilities();
			for (FGameplayAbilitySpec& ability : Abilities)
			{
				UE_LOG(LogTemp, Error, TEXT("CancelAbility %s"), *GetNameSafe(ability.Ability));
				if (ability.Ability->GetCooldownTags()->HasTagExact(OnSwordPlayingTag))
				{
					abilityComponent->CancelAbility(ability.Ability);

				}

			}

		}
	}
}

//void AInventoryItem_Sword::OnActionInputActivation()
//{
	//AMiddleFenceMainCharacterBase* mainCharacter = Cast<AMiddleFenceMainCharacterBase>(GetOwner());
	//if (mainCharacter)
	//{
	//	mainCharacter->ToggleReceiveActionInput(true);
	//}
//}

void AInventoryItem_Sword::OnSwordHit(AActor* HitActor, UPrimitiveComponent* HitComponent, FVector HitPoint, FVector HitNormal)
{
	/* Spawn Blood */
	if (SwordHitVFX && HitComponent)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(SwordHitVFX, HitComponent, NAME_None, HitPoint, HitNormal.Rotation(), EAttachLocation::Type::KeepWorldPosition, true);
	}


	/* play sword hit SFX */
	if (SwordFleshSFX)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SwordFleshSFX, HitPoint);
	}

	/*Camera Shake*/
	if (GetOwner())
	{
		AMiddleFenceMainCharacterBase* mainCharacter = Cast<AMiddleFenceMainCharacterBase>(GetOwner());
		if (mainCharacter && mainCharacter->GetController())
		{
			APlayerController* pc = Cast<APlayerController>(mainCharacter->GetController());
			pc->ClientStartCameraShake(CurrentShake, 1.0f);
			//UE_LOG(LogTemp, Warning, TEXT("ClientStartCameraShake"));
		}

	}

	if (HitActor)
	{
		AMiddleFenceCharacterBase* baseCharacter = Cast<AMiddleFenceCharacterBase>(HitActor);
		if (baseCharacter)
		{
			baseCharacter->PauseCurrentActiveAnimMonatge();
			baseCharacter->SetPositionCurrentActiveAnimMonatge(0.1f);
		}
	}
}

void AInventoryItem_Sword::OnSwordHitAfterResume(AActor* HitActor, UPrimitiveComponent* HitComponent)
{
	/*Add force to character*/
	if (HitActor && HitComponent && GetOwner())
	{
		FVector Direction = GetOwner()->GetActorLocation() - HitActor->GetActorLocation();

		if (HitComponent->IsSimulatingPhysics())
		{
			HitComponent->AddImpulse(Direction.GetSafeNormal() * -HitPower, NAME_None, true);
		}
		else
		{
			AMiddleFenceCharacterBase* baseCharacter = Cast<AMiddleFenceCharacterBase>(HitActor);
			if (baseCharacter)
			{
				baseCharacter->ResumeCurrentActiveAnimMonatge();

				/*Apply force*/
				UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(baseCharacter->GetCharacterMovement());
				if (MovementComponent)
				{
					MovementComponent->AddImpulse(Direction.GetSafeNormal() * -HitPower, true);
				}
				
				/*Character rotate toward hit sender*/
				FRotator FinalRot = Direction.Rotation();
				FinalRot.Pitch = 0;
				FinalRot.Roll = 0;
				FinalRot.Yaw -= 90;
				baseCharacter->SetActorRotation(FinalRot);
			}
		}
	}
}
