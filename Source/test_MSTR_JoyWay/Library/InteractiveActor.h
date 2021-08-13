#pragma once

#include "UObject/Interface.h"
#include "InteractiveActor.generated.h"

class AActor;

UINTERFACE(MinimalAPI, Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UInteractiveActor : public UInterface
{
	GENERATED_BODY()		
};

class TEST_MSTR_JOYWAY_API IInteractiveActor
{
	GENERATED_BODY()

	public:

	UFUNCTION(BlueprintCallable)
	virtual void Interact(class AActor* Caller) {};

	UFUNCTION(BlueprintCallable)
	virtual bool CanInteract(class AActor* Caller) { return false; }
};