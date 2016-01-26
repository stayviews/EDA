// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "EDA.h"
#include "EDACharacter.h"
#include "EDAProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Weapon/Weapon.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AEDACharacter

AEDACharacter::AEDACharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;


	CameraBoomComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoomComp->SocketOffset = FVector(0, 35, 0);
	CameraBoomComp->TargetOffset = FVector(0, 0, 55);
	CameraBoomComp->bUsePawnControlRotation = true;
	CameraBoomComp->AttachParent = GetRootComponent();


	// Create a CameraComponent	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComp->AttachParent = CameraBoomComp;
	CameraComp->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	CameraComp->bUsePawnControlRotation = true;



	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	maxHealth = 100;
	Health = maxHealth;
	bIsTargeting = false;


	/* Names as specified in the character skeleton */
	WeaponAttachPoint = TEXT("WeaponSocket");
	PelvisAttachPoint = TEXT("PelvisSocket");
	SpineAttachPoint = TEXT("SpineSocket");
}

/************************************************************************/
/* net working                                                                     */
/************************************************************************/
void AEDACharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(AEDACharacter, Inventory, COND_OwnerOnly);
	//SkipOwner
	DOREPLIFETIME_CONDITION(AEDACharacter, bIsTargeting, COND_SkipOwner);

	// everyone
	DOREPLIFETIME(AEDACharacter, CurrentWeapon);
	DOREPLIFETIME(AEDACharacter, Health);
}


void AEDACharacter::BeginPlay()
{
	Super::PostInitializeComponents();
	if (Role = ROLE_Authority)
	{
		Health = maxHealth;
		SpawnDefaultInventory();
	}
}


FRotator AEDACharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}
//////////////////////////////////////////////////////////////////////////
// Input

void AEDACharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AEDACharacter::TouchStarted);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AEDACharacter::OnStartFire);
	InputComponent->BindAction("Fire", IE_Released, this, &AEDACharacter::OnStopFire);
	
	InputComponent->BindAction("Targeting", IE_Pressed, this, &AEDACharacter::OnStartTargeting);
	InputComponent->BindAction("Targeting", IE_Released, this, &AEDACharacter::OnStopTargeting);

	InputComponent->BindAxis("MoveForward", this, &AEDACharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AEDACharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AEDACharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AEDACharacter::LookUpAtRate);
	InputComponent->BindAction("NextWeapon", IE_Pressed, this, &AEDACharacter::OnNextWeapon);
	InputComponent->BindAction("PrevWeapon", IE_Pressed, this, &AEDACharacter::OnPrevWeapon);
	InputComponent->BindAction("ToggleWeapon1", IE_Pressed, this, &AEDACharacter::ToggleWeapon1);
	InputComponent->BindAction("ToggleWeapon2", IE_Pressed, this, &AEDACharacter::ToggleWeapon2);
	InputComponent->BindAction("ToggleWeapon3", IE_Pressed, this, &AEDACharacter::ToggleWeapon3);
	

}

void AEDACharacter::OnStartFire()
{ 
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}	
}

void AEDACharacter::OnStopFire()
{
	StopWeaponFire();
}

void AEDACharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AEDACharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnStartFire();
	}
	TouchItem.bIsPressed = false;
}

void AEDACharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AEDACharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AEDACharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AEDACharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AEDACharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AEDACharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}

void AEDACharacter::OnRep_CurrentWeapon(AWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}
/************************************************************************/
/* weapon euqip toogle                                                        */
/************************************************************************/
FName AEDACharacter::GetInventoryAttachPoint(EInventorySlot Slot) const
{
	/* Return the socket name for the specified storage slot */
	switch (Slot)
	{
	case EInventorySlot::Hands:
		return WeaponAttachPoint;
	case EInventorySlot::Spine:
		return SpineAttachPoint;
	case EInventorySlot::Secondary:
		return PelvisAttachPoint;
	default:
		// Not implemented.
		return "";
	}
}
void AEDACharacter::SetCurrentWeapon(class AWeapon* NewWeapon, class AWeapon* LastWeapon)
{
	/* Maintain a reference for visual weapon swapping */
	PreviousWeapon = LastWeapon;

	AWeapon* LocalLastWeapon = nullptr;
	if (LastWeapon)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// UnEquip the current
	bool bHasPreviousWeapon = false;

	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
		bHasPreviousWeapon = true;
	}

	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip(bHasPreviousWeapon);
	}

	/* NOTE: If you don't have an equip animation w/ animnotify to swap the meshes halfway through, then uncomment this to immediately swap instead */
	//SwapToNewWeaponMesh();
}
void AEDACharacter::SwapToNewWeaponMesh()
{
	if (PreviousWeapon)
	{
		PreviousWeapon->AttachWeaponToPanwn(PreviousWeapon->GetStorageSlot());
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->AttachWeaponToPanwn(EInventorySlot::Hands);
	}
}
void AEDACharacter::EquipWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		/* Ignore if trying to equip already equipped weapon */
		if (Weapon == CurrentWeapon)
			return;
		if (Role == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

void AEDACharacter::ServerEquipWeapon_Implementation(AWeapon* Weapon)
{
	EquipWeapon(Weapon);
}

bool AEDACharacter::ServerEquipWeapon_Validate(AWeapon* Weapon)
{
	return true;
}

void AEDACharacter::OnNextWeapon()
{
	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		AWeapon* NextWeapon = Inventory[(CurrentWeaponIndex + 1) % Inventory.Num()];
		EquipWeapon(NextWeapon);
	}
}

void AEDACharacter::OnPrevWeapon()
{

	if (Inventory.Num() >= 2) // TODO: Check for weaponstate.
	{
		const int32 CurrentWeaponIndex = Inventory.IndexOfByKey(CurrentWeapon);
		AWeapon* PrevWeapon = Inventory[(CurrentWeaponIndex - 1 + Inventory.Num()) % Inventory.Num()];
		EquipWeapon(PrevWeapon);
	}
}

void AEDACharacter::ToggleWeapon1()
{
	if (Inventory[0] && CurrentWeapon->CurrentState != Equipping)
	{
		EquipWeapon(Inventory[0]);
	}
}

void AEDACharacter::ToggleWeapon2()
{
	if (Inventory[1] && CurrentWeapon->CurrentState != Equipping)
	{
		EquipWeapon(Inventory[1]);
	}
}

void AEDACharacter::ToggleWeapon3()
{
	if (Inventory[2]&&CurrentWeapon->CurrentState!=Equipping)
	{
		EquipWeapon(Inventory[2]);
	}
}
/************************************************************************/
/* targeting                                                                     */
/************************************************************************/
bool AEDACharacter::IsTargeting() const
{
	return bIsTargeting;
}

void AEDACharacter::OnStartTargeting()
{
	AEDAPlayerController* MyPC = Cast<AEDAPlayerController>(Controller);
	if (MyPC && MyPC->IsGameInputAllowed())
	{
		SetTargeting(true);
	}
}
void AEDACharacter::OnStopTargeting()
{
	SetTargeting(false);
}


void AEDACharacter::SetTargeting(bool bNewTargeting)
{
	bIsTargeting = bNewTargeting;

	if (TargetingSound)
	{
		UGameplayStatics::SpawnSoundAttached(TargetingSound, GetRootComponent());
	}

	if (Role < ROLE_Authority)
	{
		ServerSetTargeting(bNewTargeting);
	}
	
}

void AEDACharacter::ServerSetTargeting_Implementation(bool bNewTargeting)
{
	SetTargeting(bNewTargeting);
}

bool AEDACharacter::ServerSetTargeting_Validate(bool bNewTargeting)
{
	return true;
}

/************************************************************************/
/* Weapon                                                               */
/************************************************************************/
void AEDACharacter::SpawnDefaultInventory()
{
		int32 NumWeaponClasses = DefaultInventoryClasses.Num();
		for (int32 i = 0; i < NumWeaponClasses; i++)
		{
			if (DefaultInventoryClasses[i])
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultInventoryClasses[i], SpawnInfo);
				AddWeapon(NewWeapon);
			}
		}
}
void AEDACharacter::AddWeapon(class AWeapon* Weapon)
{
	
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
		// Equip first weapon in inventory
		if (Inventory.Num() > 0 && CurrentWeapon == nullptr)
		{
			EquipWeapon(Inventory[0]);
		}
}
bool AEDACharacter::IsAlive() const
{
	return Health > 0;
}
bool AEDACharacter::CanFire() const
{
	return IsAlive();
}

void AEDACharacter::StopWeaponFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}

}
float AEDACharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
                                  class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage= Super::TakeDamage(DamageAmount,DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		
		Health -= ActualDamage;
		if (Health <= 0)
		{
			Health = 0;
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}

		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);
	}
	
	return ActualDamage;
}
bool AEDACharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}
void AEDACharacter::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{

}

/************************************************************************/
/* dire respawn                                                                     */
/************************************************************************/

bool AEDACharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}

bool AEDACharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent,
                                     class AController* Killer, class AActor* DamageCauser)
{
	
	return true;
}