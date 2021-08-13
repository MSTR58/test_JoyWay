#include "EquipmentComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"														// DOREPLIFETIME
#include "DrawDebugHelpers.h"														// Drow Trace
#include "../Weapon/WeaponBase.h"
#include "../Character/PlayerCharacter.h"
#include "../Library/MSTRLibrary.h"

FName Socket_HandSlot (TEXT("Socket_HandSlot"));
FName Socket_MainSlot (TEXT("Socket_MainSlot"));
FName Socket_SecondarySlot (TEXT("Socket_SecondarySlot"));
FName Socket_KnifeSlot (TEXT("Socket_KnifeSlot"));

UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}


void UEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UEquipmentComponent, HandSlot);
	DOREPLIFETIME_CONDITION(UEquipmentComponent, MainSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UEquipmentComponent, SecondarySlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UEquipmentComponent, KnifeSlot, COND_OwnerOnly);
}


AWeaponBase* UEquipmentComponent::GetWeapon(EWeaponType Type) const
{
	switch(Type)
	{
		case EWeaponType::Main:
			return MainSlot;
		case EWeaponType::Secondary:
			return SecondarySlot;
		case EWeaponType::Knife:
			return KnifeSlot;
	}
	return nullptr;
}


FName UEquipmentComponent::GetWeaponSocket(AWeaponBase* Weapon) const
{
	if (!Weapon)
	{
		return NAME_None;
	}
	
	FWeaponPreset* Row = Weapon->GetWeaponPresetRow();
	if (!Row)
	{
		return NAME_None;
	}
	
	switch (Row->WeaponType)
	{
		case EWeaponType::Main:
			return Socket_MainSlot;
		case EWeaponType::Secondary:
			return Socket_SecondarySlot;
		case EWeaponType::Knife:
			return Socket_KnifeSlot;
		default:
			return NAME_None;
	}
}


bool UEquipmentComponent::CanEquipWeapon(AWeaponBase* Weapon) const
{
	if (!Weapon)
	{
		return false;
	}
	
	FWeaponPreset* Row = Weapon->GetWeaponPresetRow();
	
	if (!Row)
	{
		return false;
	}

	switch (Row->WeaponType)
	{
		case EWeaponType::Main:
			return MainSlot == nullptr;
		case EWeaponType::Secondary:
			return SecondarySlot == nullptr;
		case EWeaponType::Knife:
			return KnifeSlot == nullptr;
	}

	return false;
}


bool UEquipmentComponent::EquipWeapon(AWeaponBase* Weapon)
{
	const bool bResult = AddWeaponToSlot(Weapon);
	if (!bResult)
	{
		return false;
	}
	
	if (HandSlot == nullptr)
	{
		const FName SocketName = Socket_HandSlot;
		HandSlot = Weapon;
		AttachWeapon(Weapon, SocketName);
	}
	else
	{
		const FName SocketName = GetWeaponSocket(Weapon);
		AttachWeapon(Weapon, SocketName);
	}

	return true;
}


void UEquipmentComponent::DropWeapon()
{
	if (!GetOwner()->HasAuthority() || !HandSlot)
	{
		return;
	}

	FWeaponPreset* Row = HandSlot->GetWeaponPresetRow();
	if (!Row)
	{
		return;
	}

	FVector DropLocation;

	FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector()*200 + FVector{0.f,0.0f,300.f};
	FVector End = Start + FVector{0.0f, 0.0f, -1000.f};
	
	FHitResult Hit;
	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);
	
	if (bHit)
	{
		DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(3,3,3), FColor::Black, false, 5.0f);
		DropLocation = Hit.Location;
	}
	else
	{
		DropLocation = End;
	}

	HandSlot->SetOwner(nullptr);
	HandSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	HandSlot->SetActorLocation(DropLocation);
	HandSlot->WasDropped();
	HandSlot = nullptr;

	switch (Row->WeaponType)
	{
		case EWeaponType::Main:
			MainSlot = nullptr;
			break;
		case EWeaponType::Secondary:
			SecondarySlot = nullptr;
			break;
		case EWeaponType::Knife:
			KnifeSlot = nullptr;
			break;
		default:
			break;
	}
}


void UEquipmentComponent::DropWeaponServer_Implementation()
{
	DropWeapon();
}


bool UEquipmentComponent::AddWeaponToSlot(AWeaponBase* Weapon)
{
	if (!Weapon)
	{
		return false;
	}
	
	FWeaponPreset* Row = Weapon->GetWeaponPresetRow();
	
	if (!Row)
	{
		return false;
	}

	const EWeaponType WeaponType = Row->WeaponType;
	
	switch (WeaponType)
	{
		case EWeaponType::Main:
			if (MainSlot == nullptr)
			{
				MainSlot = Weapon;
				return true;
			}
			break;
		case EWeaponType::Secondary:
			if (SecondarySlot == nullptr)
			{
				SecondarySlot = Weapon;
				return true;
			}
			break;
		case EWeaponType::Knife:
			if(KnifeSlot == nullptr)
			{
				KnifeSlot = Weapon;
				return true;
			}
			break;		
		default: return false;
	}
	return false;	
}


void UEquipmentComponent::AttachWeapon(AWeaponBase* Weapon, FName SocketName) const
{
	if (!Weapon)
	{
		return;
	}
	
	APlayerCharacter* Owner = Cast<APlayerCharacter>(GetOwner());
	Weapon->AttachToComponent(Owner->GetFPSMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	Weapon->WasEquipped();
	Weapon->SetOwner(Owner->GetOwner());
}


void UEquipmentComponent::ChangeWeapon(EWeaponType Type)
{
	AWeaponBase* Weapon = GetWeapon(Type);
	if (Weapon != nullptr && Weapon != HandSlot)
	{
		if (GetOwner()->HasAuthority())
		{
			AttachWeapon(HandSlot, GetWeaponSocket(HandSlot));
			HandSlot = Weapon;
			AttachWeapon(Weapon, Socket_HandSlot);
		}
		else
		{
			ChangeWeaponServer(Type);
		}
	}
}


void UEquipmentComponent::ChangeWeaponServer_Implementation(EWeaponType Type)
{
	ChangeWeapon(Type);
}


void UEquipmentComponent::WeaponFire(bool bFire)
{
	if (!GetHandWeapon())
	{
		return;
	}
	
	if (bFire && GetHandWeapon()->CanFire())
	{
		// do shot on client and server
		GetHandWeapon()->Fire(true);

		// ask server do shot
		if (!GetOwner()->HasAuthority())
		{
			ServerWeaponFire(true);
		}
	}
	else if (!bFire)
	{
		GetHandWeapon()->Fire(false);
		// ask server stop shot
		if (!GetOwner()->HasAuthority())
		{
			ServerWeaponFire(false);
		}
	}
}


void UEquipmentComponent::ServerWeaponFire_Implementation(bool bFire)
{
	WeaponFire(bFire);
}


void UEquipmentComponent::OnPlayerDie()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	FVector DropLocation;

	FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector()*200 + FVector{0.f,0.0f,300.f};
	FVector End = Start + FVector{0.0f, 0.0f, -1000.f};
	
	FHitResult Hit;
	FCollisionQueryParams TraceParams;

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);
	
	if (bHit)
	{
		DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(3,3,3), FColor::Black, false, 5.0f);
		DropLocation = Hit.Location;
	}
	else
	{
		DropLocation = End;
	}

	HandSlot = nullptr;

	if (MainSlot)
	{
		MainSlot->SetOwner(nullptr);
		MainSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		MainSlot->SetActorLocation(DropLocation);
		MainSlot->WasDropped();
		MainSlot = nullptr;
	}

	if (SecondarySlot)
	{
		SecondarySlot->SetOwner(nullptr);
		SecondarySlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		SecondarySlot->SetActorLocation(DropLocation);
		SecondarySlot->WasDropped();
		SecondarySlot = nullptr;
	}

	if (KnifeSlot)
	{
		KnifeSlot->SetOwner(nullptr);
		KnifeSlot->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		KnifeSlot->SetActorLocation(DropLocation);
		KnifeSlot->WasDropped();
		KnifeSlot = nullptr;
	}
}










