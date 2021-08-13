#pragma once

#include "CoreMinimal.h"
#include "../Library/MSTRLibrary.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST_MSTR_JOYWAY_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	bool CanEquipWeapon(AWeaponBase* Weapon) const;
	
	bool EquipWeapon(AWeaponBase* Weapon);

	AWeaponBase* GetWeapon(EWeaponType Type) const;

	UFUNCTION(BlueprintCallable)
	AWeaponBase* GetHandWeapon() const
	{
		return HandSlot;
	}

	void ChangeWeapon(EWeaponType Type);

	UFUNCTION(Server, Reliable)
	void ChangeWeaponServer(EWeaponType Type);
	void ChangeWeaponServer_Implementation(EWeaponType Type);

	void DropWeapon();
	
	UFUNCTION(Server, Reliable)
	void DropWeaponServer();
	void DropWeaponServer_Implementation();

	void WeaponFire(bool bFire);

	UFUNCTION(Server, Reliable)
	void ServerWeaponFire(bool bFire);
	void ServerWeaponFire_Implementation(bool bFire);

	void OnPlayerDie();

private:

	FName GetWeaponSocket(AWeaponBase* Weapon) const;

	bool AddWeaponToSlot(AWeaponBase* Weapon);

	void AttachWeapon(AWeaponBase* Weapon, FName SocketName) const;
	
	UPROPERTY(Replicated)
	AWeaponBase* HandSlot = nullptr;

	UPROPERTY(Replicated)
	AWeaponBase* MainSlot = nullptr;

	UPROPERTY(Replicated)
	AWeaponBase* SecondarySlot = nullptr;

	UPROPERTY(Replicated)
	AWeaponBase* KnifeSlot = nullptr;
		
};
