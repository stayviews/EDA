// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "EDACharacter.generated.h"

class UInputComponent;
UENUM()
enum  EInventorySlot
{
	/* For currently equipped items/weapons */
	Hands,
	/* For primary weapons on spine bone */
	Spine,
	/* Storage for small items like flashlight on pelvis */
	Secondary,
};

UCLASS(config=Game)
class AEDACharacter : public ACharacter
{
	GENERATED_BODY()


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;
	/* Boom to handle distance to player mesh. */
	

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		USpringArmComponent* CameraBoomComp;
public:
	FORCEINLINE UCameraComponent* GetCameraComponent()
	{
		return CameraComp;
	}
/************************************************************************/
/* sound                                                                     */
/************************************************************************/
private:
	/** sound played when targeting state changes */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* TargetingSound;
	/** sound played on respawn */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RespawnSound;

	/** sound played when health is low */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* LowHealthSound;

	/** sound played when running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RunLoopSound;

	/** sound played when stop running */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
		USoundCue* RunStopSound;

/************************************************************************/
/* animation                                                                     */
/************************************************************************/

public:

	
	AEDACharacter();
	//////////////////////////////////////////////////////////////////////////
	//Character State
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Health)
	float Health;
	UPROPERTY(EditDefaultsOnly, Category = ChracterState)
	float sprintSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ChracterState)
	float maxHealth;

	/** spawn inventory, setup initial variables */
	virtual void PostInitializeComponents() override;
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AEDAProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** get aim offsets */
	UFUNCTION(BlueprintCallable, Category = "Game|Weapon")
		FRotator GetAimOffsets() const;
protected:
	
	/** Fires a projectile. */
	void OnStartFire();
	
	void OnStopFire();
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface



public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComp; }
	UFUNCTION(BlueprintCallable, Category = Mesh)
	virtual bool IsFirstPerson() const;
/************************************************************************/
/* weapon spawn equip tooggle                                                                     */
/********************************** **************************************/
private:
	
	/* Attachpoint for active weapon/item in hands */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName WeaponAttachPoint;
	/* Attachpoint for items carried on the belt/pelvis. */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName PelvisAttachPoint;
	/* Attachpoint for primary weapons */
	UPROPERTY(EditDefaultsOnly, Category = "Sockets")
		FName SpineAttachPoint;
public:
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TArray<TSubclassOf<class AWeapon>> DefaultInventoryClasses;
	UPROPERTY(Transient, EditDefaultsOnly, Replicated)
	TArray<class AWeapon*> Inventory;
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon)
	class AWeapon* CurrentWeapon;
	class AWeapon* SpineWeapon;
	class AWeapon* PreviousWeapon;


public:
	UFUNCTION()
	void OnRep_CurrentWeapon();
	FName GetInventoryAttachPoint(EInventorySlot Slot) const;
	void SpawnDefaultInventory();
	void AddWeapon(class AWeapon* Weapon);
	//change current Weapon
	void EquipWeapon(AWeapon* Weapon);
	UFUNCTION(Reliable,Server,WithValidation)
	void ServerEquipWeapon(AWeapon* Weapon);
	void ServerEquipWeapon_Implementation(AWeapon* Weapon);
	bool ServerEquipWeapon_Validate(AWeapon* Weapon);
	
	void SetCurrentWeapon(class AWeapon* NewWeapon);
	void OnNextWeapon();
	void OnPrevWeapon();
	void ToggleWeapon1();
	void ToggleWeapon2();
	void ToggleWeapon3();
/************************************************************************/
/* shoot                                                                     */
/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsFiring() const;
	bool IsAlive() const;
	bool CanFire() const;
	void StopWeaponFire();
//////////////////////////////////////////////////////////////////////////
//targeting
	UPROPERTY(BlueprintReadWrite,Transient, Replicated)
	uint8 bIsTargeting : 1;
	bool IsTargeting() const;
	void OnStartTargeting();
	void OnStopTargeting();
	/** [server + local] change targeting state */
	void SetTargeting(bool bNewTargeting);
	/** update targeting state */
	UFUNCTION(reliable, server, WithValidation)
	void ServerSetTargeting(bool bNewTargeting);
	void ServerSetTargeting_Implementation(bool bNewTargeting);
	bool ServerSetTargeting_Validate(bool bNewTargeting);
//////////////////////////////////////////////////////////////////////////
//Damage

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	
	
	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);
/************************************************************************/
/* die respawn                                                                     */
/************************************************************************/
/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint8 bIsDying : 1;

/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;
/*
* Kills pawn.  Server/authority only.
* @param KillingDamage - Damage amount of the killing blow
* @param DamageEvent - Damage event of the killing blow
* @param Killer - Who killed this pawn
* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
* @returns true if allowed
*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);
};