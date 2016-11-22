// Copyright 2015-2017 Piperift. All Rights Reserved.

#pragma once

#include "Components/ArrowComponent.h"
#include "LIAnchorViewerComponent.generated.h"

class ALevelInstanceBounds;
struct FLIAnchor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JINKCORE_API ULIAnchorViewerComponent : public UArrowComponent
{
	GENERATED_BODY()

public:
	FGuid AnchorGUID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Anchor")
	FName Name;

	// Sets default values for this component's properties
	ULIAnchorViewerComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

#if WITH_EDITOR
	virtual void PostEditComponentMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR

private:
	FORCEINLINE ALevelInstanceBounds* GetLIBoundsActor() {
		return Cast<ALevelInstanceBounds>(GetOwner());
	}

	FLIAnchor* GetLIAnchor() {
		if (ALevelInstanceBounds* LIBoundsActor = GetLIBoundsActor()) {
			return LIBoundsActor->GetAnchorByGUID(AnchorGUID);
		}
		return nullptr;
	}
	
};
