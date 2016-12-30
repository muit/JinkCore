// Copyright 2015-2016 Piperift. All Rights Reserved.

#pragma once

#include "Object.h"
#include "LenguajeData.generated.h"

/** Contains all the lenguaje data. */
USTRUCT()
struct FLenguajeData
{
    GENERATED_USTRUCT_BODY()

    /** Lenguaje Name */
    UPROPERTY(EditAnywhere)
    FString Name;

    /** Texts Datatable */
    UPROPERTY(EditAnywhere, meta = (AllowedClasses = "DataTable"))
    FStringAssetReference Texts;

    /** Audios Datatable */
    UPROPERTY(EditAnywhere, meta = (AllowedClasses = "DataTable"))
    FStringAssetReference Audios;
};

/** Struct containing a lenguaje text. */
USTRUCT(BlueprintType)
struct FLenguajeTextRow
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere)
    FString Text;
};

/** Struct containing a lenguaje audio. */
USTRUCT(BlueprintType)
struct FLenguajeAudioRow
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, meta = (AllowedClasses = "SoundWave"))
    FStringAssetReference Audio;
};
