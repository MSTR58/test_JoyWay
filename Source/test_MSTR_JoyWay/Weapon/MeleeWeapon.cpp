#include "MeleeWeapon.h"
#include "DrawDebugHelpers.h"
#include "../Library/MSTRLibrary.h"

void AMeleeWeapon::Fire(bool bFire)
{
	if (bFire)
	{
		FVector Start = GetActorLocation();
		FVector End = Start + GetOwner()->GetActorForwardVector() * 100.f;
	
		FHitResult Hit;
		FCollisionQueryParams TraceParams;

		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, TraceParams);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f);
	
		if (bHit)
		{
			DrawDebugSolidBox(GetWorld(), Hit.ImpactPoint, FVector(3,3,3), FColor::Black, false, 5.0f);
			if (HasAuthority())
			{
				FWeaponPreset* Row = GetWeaponPresetRow();
				AController* Controller = Cast<AController>(GetOwner());
				
				if (!Row)
				{
					return;
				}
				
				FDamageEvent DamageEvent;
				Hit.Actor->TakeDamage(Row->BaseDamage, DamageEvent, Controller, this);
			}
		}
	}
}
