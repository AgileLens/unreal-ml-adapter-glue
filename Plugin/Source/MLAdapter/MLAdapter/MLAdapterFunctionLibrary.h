#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "MLAdapterFunctionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FMLPrediction
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "ML Prediction")
    FVector NewLocation;
};

UCLASS()
class UMLAdapterFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Sends the given Actor's transform to the external ML process and returns the predicted new location.
     * Returns a zero vector if the connection is unavailable or parsing fails.
     */
    UFUNCTION(BlueprintCallable, Category = "ML Adapter")
    static FMLPrediction SendActorState(AActor* Actor);
};
