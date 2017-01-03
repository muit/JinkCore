// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.


#pragma once
#include "PhysicsEngine/RigidBodyBase.h"
#include "LinearForceActor.generated.h"

UCLASS(hideCategories=(Collision, Input), showCategories=("Input|MouseInput", "Input|TouchInput"), ComponentWrapperClass)
class ALinearForceActor : public ARigidBodyBase
{
	GENERATED_UCLASS_BODY()

private_subobject:
	/** Force component */
	DEPRECATED_FORGAME(4.6, "ForceComponent should not be accessed directly, please use GetForceComponent() function instead. ForceComponent will soon be private and your code will not compile.")
	UPROPERTY(Category = LinearForceActor, VisibleAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Activation,Components|Activation,Physics,Physics|Components|RadialForce", AllowPrivateAccess = "true"))
	class ULinearForceComponent* ForceComponent;

#if WITH_EDITORONLY_DATA
	DEPRECATED_FORGAME(4.6, "SpriteComponent should not be accessed directly, please use GetSpriteComponent() function instead. SpriteComponent will soon be private and your code will not compile.")
	UPROPERTY()
	UBillboardComponent* SpriteComponent;
#endif
public:

	// BEGIN DEPRECATED (use component functions now in level script)
	UFUNCTION(BlueprintCallable, Category="Physics", meta=(DeprecatedFunction))
	virtual void FireImpulse();
	UFUNCTION(BlueprintCallable, Category="Physics", meta=(DeprecatedFunction))
	virtual void EnableForce();
	UFUNCTION(BlueprintCallable, Category="Physics", meta=(DeprecatedFunction))
	virtual void DisableForce();
	UFUNCTION(BlueprintCallable, Category="Physics", meta=(DeprecatedFunction))
	virtual void ToggleForce();
	// END DEPRECATED


#if WITH_EDITOR
	//~ Begin AActor Interface.
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
	//~ End AActor Interface.
#endif

public:
	/** Returns ForceComponent subobject **/
	JINKCORE_API class ULinearForceComponent* GetForceComponent() const;
#if WITH_EDITORONLY_DATA
	/** Returns SpriteComponent subobject **/
    JINKCORE_API UBillboardComponent* GetSpriteComponent() const;
#endif
};



