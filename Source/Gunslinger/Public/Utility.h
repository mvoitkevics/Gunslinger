// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Utility.generated.h"

/**
 * 
 */
UCLASS()
class GUNSLINGER_API UUtility : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

    
    template <typename T>
    UFUNCTION(BlueprintCallable, Category = Arrays)
    FORCEINLINE static TArray<T> ShuffleArray(TArray<T> InArray, int seed)
    {
        srand(seed);
        int x = InArray.Num();

        for (int i = 0; i < x; i++)
        {
            int index = rand() % x;
            FString temp = InArray[i];
            InArray[i] = InArray[index];
            InArray[index] = temp;
        }

        return InArray;
    }
};
