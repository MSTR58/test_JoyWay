#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Weapon/WeaponProjectile.h"
#include "../Library/InteractiveActor.h"
#include "WeaponBase.generated.h"

class USkeletalMeshComponent;
class UDataTable;
struct FWeaponPreset;

UCLASS()
class TEST_MSTR_JOYWAY_API AWeaponBase : public AActor, public IInteractiveActor
{
	GENERATED_BODY()
	
public:	
	
	AWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void OnRep_AttachmentReplication() override;

	virtual void Interact(AActor* Caller) override;

	virtual bool CanInteract(AActor* Caller) override;

	virtual bool CanFire() const;

	virtual void Fire(bool bFire);

	UFUNCTION(BlueprintCallable)
	void WasSpawned();

	void WasEquipped();

	void WasDropped();

	void Init();

	UFUNCTION()
	void OnRep_RowName();

	USkeletalMeshComponent* GetMesh() const
	{
		return Mesh;
	}

	FName GetRowName() const
	{
		return RowName;
	}

	FWeaponPreset* GetWeaponPresetRow() const;
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Utility", meta = (AllowPrivateAccess = "true"))
	UDataTable* WeaponPresetDT = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_RowName,
											meta = (AllowPrivateAccess="true"), meta = (ExposeOnSpawn="true"))
	FName RowName = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AWeaponProjectile> ProjectileClass;
};
