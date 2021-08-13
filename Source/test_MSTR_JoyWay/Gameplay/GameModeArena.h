#pragma once

#include "CoreMinimal.h"

#include "PlayerControllerArena.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeArena.generated.h"

UCLASS()
class TEST_MSTR_JOYWAY_API AGameModeArena : public AGameModeBase
{
	GENERATED_BODY()

	public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void RespawnCharacter(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SpawnWeapon(FName RowName, APlayerControllerArena* PlayerController);	
};
