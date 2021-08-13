#pragma once

#include "CoreMinimal.h"

#include "EquipmentComponent.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UEquipmentComponent;

UCLASS()
class TEST_MSTR_JOYWAY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	APlayerCharacter();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	USkeletalMeshComponent* GetFPSMesh() const
	{
		return FPSMesh;
	}

	USkeletalMeshComponent* GetTPSMesh() const
	{
		return GetMesh();
	}

	UCameraComponent* GetCamera() const
	{
		return Camera;
	}

	UEquipmentComponent* GetEquipmentComponent() const
	{
		return Equipment;
	}

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth)
	{
		Health = NewHealth;
	}

protected:
	
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void LookUp(float Value);

	void LookRight(float Value);

	AActor* LineTraceForward();

	FVector GetCameraLocation() const;

	void Interact();

	UFUNCTION(Server, Reliable)
	void ServerInteract();
	void ServerInteract_Implementation();

	void SelectMainWeapon();

	void SelectSecondaryWeapon();

	void SelectKnife();

	void DropWeapon();

	void WeaponFire();

	void StopWeaponFire();

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPSMesh", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* FPSMesh = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	class UEquipmentComponent* Equipment = nullptr;

	UPROPERTY(BlueprintReadOnly, Replicated, meta = (AllowPrivateAccess = "true"))
	float ReplicatedControllerPitch;

	UPROPERTY(Replicated, ReplicatedUsing = OnRep_Health)
	float Health = 100.f;
};
