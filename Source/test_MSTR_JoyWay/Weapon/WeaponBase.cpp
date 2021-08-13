#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Library/MSTRLibrary.h"
#include "Engine/DataTable.h"
#include "../Character/PlayerCharacter.h"
#include "../Character/EquipmentComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"														// DOREPLIFETIME

FName Socket_GunMuzzle (FName(TEXT("Socket_GunMuzzle")));

AWeaponBase::AWeaponBase()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	NetCullDistanceSquared = 60000000.0f;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = Mesh;
	//Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}


void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeaponBase, RowName);
}


void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	SetReplicateMovement(true);
	Init();
}


void AWeaponBase::OnRep_AttachmentReplication()
{
	Super::OnRep_AttachmentReplication();
	APlayerCharacter* CharacterOwner = Cast<APlayerCharacter>(GetAttachParentActor());
	
	if (CharacterOwner)
	{
		WasEquipped();
		
		if (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
		{
			const FName Socket = GetAttachParentSocketName();
			AttachToComponent(CharacterOwner->GetTPSMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
		}
	}
	else
	{
		WasDropped();
	}
}


void AWeaponBase::OnRep_RowName()
{
	Init();
}


void AWeaponBase::Init()
{
	if (WeaponPresetDT == nullptr)
	{
		return;
	}

	FWeaponPreset* Row = GetWeaponPresetRow();

	if (Row)
	{
		GetMesh()->SetSkeletalMesh(Row->SkeletalMesh);
	}
	else
	{
		GetMesh()->SetSkeletalMesh(nullptr);
	}	
}


void AWeaponBase::WasSpawned()
{
	if (WeaponPresetDT != nullptr && GetLocalRole() == ROLE_Authority)
	{
		FWeaponPreset* Row = GetWeaponPresetRow();

		if (Row)
		{
			SetActorLocation(GetActorLocation() + Row->OffSetTransform.GetLocation());
			SetActorRotation(GetActorRotation() + Row->OffSetTransform.GetRotation().Rotator());
		}
	}
}


void AWeaponBase::WasEquipped()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void AWeaponBase::WasDropped()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (WeaponPresetDT != nullptr && GetLocalRole() == ROLE_Authority)
	{
		FWeaponPreset* Row = GetWeaponPresetRow();

		if (Row)
		{
			SetActorLocation(GetActorLocation() + Row->OffSetTransform.GetLocation());
			SetActorRotation(GetActorRotation() + Row->OffSetTransform.GetRotation().Rotator());
		}
	}
}


FWeaponPreset* AWeaponBase::GetWeaponPresetRow() const
{
	const FString ContextString = GetFullName();
	return WeaponPresetDT->FindRow<FWeaponPreset>(RowName, ContextString, true);	
}


void AWeaponBase::Interact(AActor* Caller)
{
	if (GetOwner())
	{
		return;
	}

	if (HasAuthority())
	{
		APlayerCharacter* Character = Cast<APlayerCharacter>(Caller);
		if (Character)
		{
			Character->GetEquipmentComponent()->EquipWeapon(this);
		}
	}
}


bool AWeaponBase::CanInteract(AActor* Caller)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(Caller);
	if (Character)
	{
		return  Character->GetEquipmentComponent()->CanEquipWeapon(this);
	}

	return false;
}


bool AWeaponBase::CanFire() const
{
	return true;
}


void AWeaponBase::Fire(bool bFire)
{
	if (!bFire)
	{
		return;
	}
	
	UWorld* const World = GetWorld();
	APlayerController* Controller = Cast<APlayerController>(GetOwner());
	
	if (ProjectileClass != nullptr && World != nullptr && Controller != nullptr)
	{
		const FRotator SpawnRotation = Controller->GetControlRotation();
		const FVector SpawnLocation = GetMesh()->GetSocketLocation(Socket_GunMuzzle);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Owner = GetOwner();
		ActorSpawnParams.Instigator = GetInstigator();

		FWeaponPreset* Row = GetWeaponPresetRow();
		AWeaponProjectile* Projectile = World->SpawnActor<AWeaponProjectile>(ProjectileClass,
																SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Projectile && Row)
		{
			Projectile->SetDamage(Row->BaseDamage);
		}
	}
}










