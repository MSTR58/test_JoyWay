#include "PlayerControllerArena.h"
#include "GameModeArena.h"
#include "Kismet/GameplayStatics.h"

void APlayerControllerArena::SpawnWeapon(FName RowName)
{
	if (!HasAuthority())
	{
		return;
	}
	
	AGameModeArena* GameMode = Cast<AGameModeArena>(UGameplayStatics::GetGameMode(this));
	
	if (GameMode)
	{
		GameMode->SpawnWeapon(RowName, this);
	}
}

void APlayerControllerArena::SpawnWeaponServer_Implementation(FName RowName)
{
	SpawnWeapon(RowName);
}

