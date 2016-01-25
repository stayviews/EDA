// Fill out your copyright notice in the Description page of Project Settings.

#include "EDA.h"
#include "EdaImpactEffect.h"


// Sets default values
AEdaImpactEffect::AEdaImpactEffect()
{
	bAutoDestroyWhenFinished = true;
	bReplicates = true;
}

void AEdaImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// play sound
// 	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
// 	if (ImpactSound)
// 	{
// 		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
// 	}
// 
// 	if (DefaultDecal.DecalMaterial)
// 	{
// 		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
// 		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);
// 
// 		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(DefaultDecal.DecalSize, DefaultDecal.DecalSize, 1.0f),
// 			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
// 			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
// 			DefaultDecal.LifeSpan);
// 	}
}
UParticleSystem* AEdaImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = NULL;

	switch (SurfaceType)
	{
	case SHOOTER_SURFACE_Concrete:	ImpactFX = ConcreteFX; break;
	case SHOOTER_SURFACE_Dirt:		ImpactFX = DirtFX; break;
	case SHOOTER_SURFACE_Water:		ImpactFX = WaterFX; break;
	case SHOOTER_SURFACE_Metal:		ImpactFX = MetalFX; break;
	case SHOOTER_SURFACE_Wood:		ImpactFX = WoodFX; break;
	case SHOOTER_SURFACE_Grass:		ImpactFX = GrassFX; break;
	case SHOOTER_SURFACE_Glass:		ImpactFX = GlassFX; break;
	case SHOOTER_SURFACE_Flesh:		ImpactFX = FleshFX; break;
	default:						ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}



