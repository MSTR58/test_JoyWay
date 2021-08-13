#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "MeleeWeapon.generated.h"

UCLASS()
class TEST_MSTR_JOYWAY_API AMeleeWeapon : public AWeaponBase
{
	GENERATED_BODY()

public:

	virtual void Fire(bool bFire) override;
};
