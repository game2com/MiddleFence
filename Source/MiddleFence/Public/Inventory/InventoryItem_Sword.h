// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/MiddleFenceInventoryBase.h"
#include "Inventory/MiddleFenceSwordDataAsset.h"
#include "Game/MiddleFenceDataTypes.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "InventoryItem_Sword.generated.h"



/************************************************************************/
/*                                                                      */
/************************************************************************/
UCLASS()
class MIDDLEFENCE_API AInventoryItem_Sword : public AMiddleFenceInventoryBase
{
	GENERATED_BODY()
	
public:

	/************************************************************************/
	/* Set default properties                                               */
	/************************************************************************/
	AInventoryItem_Sword();

	/************************************************************************/
	/* Sword mesh component                                                 */
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Component)
	UStaticMeshComponent* SwordMeshComponent;

protected:

	/************************************************************************/
	/* Which bone in the character is responsible for trace, like hand_r	*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FName CharacterHandBoneForTrace;

	/************************************************************************/
	/* Which objects can accept trace										*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	TArray<TEnumAsByte<ECollisionChannel>> TraceObjectTypes;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FGameplayTag SendEventTag_OnTrace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setup")
	FGameplayTag OnSwordPlayingTag;


	/************************************************************************/
	/* Trace Radius for checking if sword hit something						*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Trace")
	float TraceRadius;

	/************************************************************************/
	/*  Trace Size for checking if sword hit something						*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Trace")
	float TraceLength;


	/************************************************************************/
	/* Times that trace happen												*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Trace")
	TArray<float> TraceTimeSampling;


	/************************************************************************/
	/* It is an index for TraceTimeSampling[]                                 */
	/************************************************************************/
	int SwordTraceCounter;


	/************************************************************************/
	/* Delta time between TraceTimeSampling[i] and TraceTimeSampling[i+1]	*/
	/************************************************************************/
	float NextTraceTime;



	/************************************************************************/
	/* How long wait after the sword hit anything							*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, Category = "Trace")
	float FramePauseTime = 0.1f;


	/************************************************************************/
	/* In each animation apply hit on object once							*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, Category = "Trace")
	TArray<AActor*> ExcludeTraceActors;


	/************************************************************************/
	/* Timer assets for only single trace									*/
	/************************************************************************/
	FTimerHandle TimerHandle_SingleTrace;
	FTimerDelegate TimerDelegate_SingleTrace;



	/************************************************************************/
	/* Timer assets for resuming animation									*/
	/************************************************************************/
	FTimerHandle TimerHandle_ResumeMontage;


	/************************************************************************/
	/* Is true when trace process starts									*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, Category = "Trace", BlueprintReadOnly)
	bool bIsTracing;


	FTimerHandle TimerHandle_SendHitDataAfterResume;
	FTimerDelegate TimerDelegate_SendHitDataAfterResume;

	/************************************************************************/
	/* Store all data about combos											*/
	/* it is a file stored in the content browser							*/
	/************************************************************************/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	UMiddleFenceSwordDataAsset* ComboAsset;


	/************************************************************************/
	/* Get all data from ComboAsset and save here for direct access			*/
	/************************************************************************/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combo")
	TArray<FSwordTraceAnimationDataRow> ComboData;


	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combo")
	FSwordTraceAnimationDataRow CurrentSwordTraceData;


	/************************************************************************/
	/* Indicate index of [ComboData]										*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int ComboCounter;


	/************************************************************************/
	/* Show debug lines?                                                    */
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	bool bDrawTrace;


	/************************************************************************/
	/* let the single animation has a debug lines. 
	/* update by bForceEnableTraceDebug in combo asset						*/
	/************************************************************************/
	bool bDrawEachAnimationTraceDebug = false;


	/************************************************************************/
	/* Color of trace debug line                                            */
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	FColor TraceDrawColor = FColor::White;


	/************************************************************************/
	/* How long show trace lines?                                           */
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
	float DrawliveTime = 5.0f;



	/************************************************************************/
	/* SFX on hit															*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	USoundCue* SwordFleshSFX;


	/************************************************************************/
	/* VFX on hit															*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* SwordHitVFX;


	/************************************************************************/
	/* Default Camera shake Template class on hit							*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TSubclassOf<class UCameraShakeBase> DefaultShake;


	/************************************************************************/
	/* store Current Camera shake Template class. it can be override on
	/* Combo asset file.													*/
	/************************************************************************/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	TSubclassOf<class UCameraShakeBase> CurrentShake;


	/************************************************************************/
	/* How much the hit push the hitted actor.
	/* Sword power to push actors											*/
	/************************************************************************/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	float HitPower;


public:

	/************************************************************************/
	/* A good place for initialize data										*/
	/************************************************************************/
	void OnAfterSpawning() override;

	/************************************************************************/
	/* Check if sword can be executed                                       */
	/************************************************************************/
	AMiddleFenceInventoryBase* OnAction(TArray<FComboInput>InputBuffer) override;

protected:

	/************************************************************************/
	/* Base place to unbind delegates										*/
	/************************************************************************/
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	void InitializeSwordData(FSwordTraceAnimationDataRow comboData);


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//void ManageCombo();


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	bool ManageComboInput(TArray<FComboInput>InputBuffer);

	

public:

	/************************************************************************/
	/* Get called by Anim_Notify											*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Sword")
	void ClearExcludeTraceActors();

	/************************************************************************/
	/* Return current combo animation										*/
	/************************************************************************/
	UAnimMontage* GetCurrentComboAnimMontage() const;


	/************************************************************************/
	/* Return the distance to target. get from combo asset                  */
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Sword")
	float GetWarpMotionDistanceFromTarget() const;

	/************************************************************************/
	/* Get called by Anim_Notify											*/
	/************************************************************************/
	UFUNCTION(BlueprintCallable, Category = "Sword")
	void TryTrace();

protected:

	/************************************************************************/
	/* Main place for running trace                                         */
	/************************************************************************/
	void DoSingleTrace(USkeletalMeshComponent* PlayerMesh);


	/************************************************************************/
	/* Resume animation after a short pause after hit                       */
	/************************************************************************/
	void ResumeMontage();

public:

	/************************************************************************/
	/* Event                                                                */
	/************************************************************************/
	void OnSwordAnimationStart();

	/************************************************************************/
	/* Event                                                                */
	/************************************************************************/
	void OnSwordAnimationEnd();

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	FDelegateHandle DelegateHandle_OnAnimationPossibleEnd;
	virtual void OnAnimationPossibleEnd() override;


	/************************************************************************/
	/* Force Cancel															*/
	/************************************************************************/
	virtual void ForceCancel() override;

protected:

	/************************************************************************/
	/* Call when sword hit an actor                                         */
	/************************************************************************/
	void OnSwordHit(AActor* HitActor, UPrimitiveComponent* HitComponent, FVector HitPoint, FVector HitNormal);

	/***************************************************************************************/
	/* Call when sword hit an actor then wait till the character current montage resumed   */
	/***************************************************************************************/
	void OnSwordHitAfterResume(AActor* HitActor, UPrimitiveComponent* HitComponent);
};
