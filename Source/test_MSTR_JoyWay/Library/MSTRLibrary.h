#pragma once

#include "Animation/AnimInstance.h"
#include "Engine/DataTable.h"
#include "MSTRLibrary.generated.h"

class UCurveFloat;
class UCurveVector;

// Weapon Type
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Main				UMETA(DisplayName = "Main"),
	Secondary			UMETA(DisplayName = "Secondary"),
	Knife				UMETA(DisplayName = "Knife")
};

// Weapon Preset
USTRUCT(Blueprintable)
struct FWeaponPreset : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FTransform OffSetTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AWeaponBase> WeaponClass;
};