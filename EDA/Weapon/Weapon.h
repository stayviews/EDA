// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM()
enum EWeaponState
{
	Idle,
	Firing,
	Reloading,
	Equipping,
};
UENUM(BlueprintType)
enum  EAmmoType
{
	EBullet,
	ESpread,
};
USTRUCT()
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

		/** inifite ammo for reloads */
		UPROPERTY(EditDefaultsOnly, Category = Ammo)
		bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		bool bInfiniteClip;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category = WeaponStat)
		float TimeBetweenShots;
	/* Time to assign on reload when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		float NoAnimReloadDuration;



	/** defaults */
	FWeaponData()
	{
		bInfiniteAmmo = false;
		bInfiniteClip = false;
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		NoAnimReloadDuration = 1.0f;
	}
};
UCLASS()
class EDA_API AWeapon : public AActor
{
	GENERATED_BODY()
	
protected:	
	// Sets default values for this actor's properties
	AWeapon();
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FWeaponData WeaponConfig;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* weaponMesh;
	

	void DetermineWeaponState(EWeaponState NewState);
/************************************************************************/
/* effect                                                                     */
/************************************************************************/	
private:
//////////////////////////////////////////////////////////////////////////
//sound
		/** firing audio (bLoopedFireSound set) */
		UPROPERTY(Transient)
		UAudioComponent* FireAC;

		/** name of bone/socket for muzzle in weapon mesh */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName MuzzleAttachPoint;

		/** FX for muzzle flash */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* MuzzleFX;

		/** spawned component for muzzle FX */
		UPROPERTY(Transient)
			UParticleSystemComponent* MuzzlePSC;

		/** spawned component for second muzzle FX (Needed for split screen) */
		UPROPERTY(Transient)
			UParticleSystemComponent* MuzzlePSCSecondary;

		/** camera shake on firing */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
			TSubclassOf<UCameraShake> FireCameraShake;

		/** force feedback effect to play when the weapon is fired */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
			UForceFeedbackEffect *FireForceFeedback;

		/** single fire sound (bLoopedFireSound not set) */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
			USoundCue* FireSound;

		/** looped fire sound (bLoopedFireSound set) */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
			USoundCue* FireLoopSound;

		/** finished burst sound (bLoopedFireSound set) */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
			USoundCue* FireFinishSound;

		/** out of ammo sound */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
			USoundCue* OutOfAmmoSound;

		/** reload sound */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* ReloadSound;
		/** equip sound */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
		USoundCue* EquipSound;
		/** reload animations */
		
//////////////////////////////////////////////////////////////////////////
//animation
		
		UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* ReloadAnim;

		/** equip animations */
		UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* EquipAnim;

	    UPROPERTY(EditDefaultsOnly, Category = Animation)
	    UAnimMontage* FireAnim;

		/** is muzzle FX looped? */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
		uint32 bLoopedMuzzleFX : 1;

		/** is fire sound looped? */
		UPROPERTY(EditDefaultsOnly, Category = Sound)
		uint32 bLoopedFireSound : 1;

		/** is fire animation looped? */
		UPROPERTY(EditDefaultsOnly, Category = Animation)
		uint32 bLoopedFireAnim : 1;

		/** is fire animation playing? */
		uint32 bPlayingFireAnim : 1;

		/** is weapon currently equipped? */
		uint32 bIsEquipped : 1;

		/** is weapon fire active? */
		uint32 bWantsToFire : 1;
public:
	/** get the muzzle location of the weapon */
	FVector GetMuzzleDirection() const;
	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;
	
	

	/** Called in network play to do the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** Called in network play to stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();
	/** play weapon animations */
<<<<<<< HEAD
	float PlayWeaponAnimation(UAnimMontage* Animation,float InRate=1);
=======
	float PlayWeaponAnimation(UAnimMontage* Animation,float InPlayRate=1.0f);
>>>>>>> origin/master

	/** stop playing weapon animations */
	void StopWeaponAnimation(UAnimMontage* Animation);
public:
/************************************************************************/
/* pawn                                                                */
/************************************************************************/
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class AEDACharacter* MyPawn;
	UFUNCTION()
	void OnRep_MyPawn();
	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TEnumAsByte<EInventorySlot> StorageSlot;
	//Attach mesh ToPanwn
	void AttachWeaponToPanwn(EInventorySlot Slot = EInventorySlot::Hands);
	void DetachMeshFromPawn();
//////////////////////////////////////////////////////////////////////////
//equip
   /** last time when this weapon was switched to */
	float EquipStartedTime;
	/** how much time weapon needs to be equipped */
	float EquipDuration; 
	/* Time to assign on equip when no animation is found */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float NoEquipAnimDuration;
	uint8 bPendingEquip : 1;
	
	FTimerHandle EquipFinishedTimerHandle;
	
	FTimerHandle SwapWeaponFinishedTimerHandle;
	
	void OnEquip(bool bPlayAnimation);
	void OnEquipFinished();
	void OnSwapWeaponFinished();
	void OnUnEquip();
	FORCEINLINE EInventorySlot GetStorageSlot()
	{
		return StorageSlot;
	}
/************************************************************************/
/* get weapon data                                                                     */
/************************************************************************/
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;
	/** current ammo - inside clip */
	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

/** get current weapon state */
	EWeaponState GetCurrentState() const;

	/** get current ammo amount (total) */
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	int32 GetMaxAmmo() const;
	
	USkeletalMeshComponent* GetWeaponMesh() const;
/************************************************************************/
/* Input ServerSide                                                                     */
/************************************************************************/
	void StartFire();
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate();
	void StopFire();
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();
	

	UPROPERTY(EditDefaultsOnly, Category = Config)
	TEnumAsByte<EWeaponState> CurrentState;
	
	UFUNCTION()
	void SetOwningPawn(class AEDACharacter* NewOwner);
	UFUNCTION()
	void OnEnterInventory(class AEDACharacter* NewOwner);
/************************************************************************/
/* target                                                                   */
/************************************************************************/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	float targetingFOV;
	FVector GetAdjustedAim() const;
	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;
/************************************************************************/
/*Fire                                                                      */
/************************************************************************/	

/** weapon is refiring */
	uint8 bRefiring:1;
//////////////////////////////////////////////////////////////////////////
/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AWeapon::FireWeapon,);

/** check if weapon can fire */
	bool CanFire() const;

	/** check if weapon can be reloaded */
	bool CanReload() const;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_BurstCounter)
	int32 BurstCounter;
	UFUNCTION()
	void OnRep_BurstCounter();
/** time of last successful weapon fire */
	float LastFireTime;

	
	void SetWeaponState(EWeaponState NewState);
	/** [local + server] firing started */
	void OnBurstStarted();
	
	void OnBurstFinished();
	
	/** [local + server] handle weapon fire */
	void HandleFiring();
	/** [server] fire & update ammo */
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate();

	/** instant hit notify for replication */

	
	/** called in network play to do the cosmetic fx  */
	void SimulateInstantHit(const FVector& Origin, int32 RandomSeed, float ReticleSpread);
//////////////////////////////////////////////////////////////////////////
	//time handle
	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;
/************************************************************************/
/* Ammo                                                                     */
/************************************************************************/
	/** consume a bullet */
	void UseAmmo();

	void ProcessInstantHit_Confirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandomSeed, float ReticleSpread);
	

};

