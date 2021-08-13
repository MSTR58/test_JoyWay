#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerArena.generated.h"

UCLASS()
class TEST_MSTR_JOYWAY_API APlayerControllerArena : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SpawnWeapon(FName RowName);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void SpawnWeaponServer(FName RowName);
	void SpawnWeaponServer_Implementation(FName RowName);
};
