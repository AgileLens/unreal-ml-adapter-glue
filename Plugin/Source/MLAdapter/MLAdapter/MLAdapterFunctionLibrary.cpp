#include "MLAdapterFunctionLibrary.h"
#include "MLAdapter.h"
#include "Engine/World.h"
#include "Json.h"
#include "JsonUtilities.h"

FMLPrediction UMLAdapterFunctionLibrary::SendActorState(AActor* Actor)
{
    FMLPrediction Result;
    Result.NewLocation = FVector::ZeroVector;
    if (!Actor) return Result;

    // Build JSON payload
    TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    JsonObject->SetStringField("actor", Actor->GetName());
    FVector Loc = Actor->GetActorLocation();
    TSharedRef<FJsonObject> PosObj = MakeShared<FJsonObject>();
    PosObj->SetNumberField("x", Loc.X);
    PosObj->SetNumberField("y", Loc.Y);
    PosObj->SetNumberField("z", Loc.Z);
    JsonObject->SetObjectField("position", PosObj);

    // Serialize to string
    FString OutString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutString);
    FJsonSerializer::Serialize(JsonObject, Writer);

    // Send via the global module (simple static access for demo)
    // Use the static accessor to get the socket from the module instance.
    FSocket* Sock = FMLAdapterModule::GetConnectionSocket();
    if (Sock && Sock->GetConnectionState() == ESocketConnectionState::SCS_Connected)
    {
        // Send JSON
        FTCHARToUTF8 Convert(*OutString);
        int32 Sent = 0;
        Sock->Send((uint8*)Convert.Get(), Convert.Length(), Sent);

        // Receive response (blocking, short timeout)
        uint32 Size;
        if (Sock->HasPendingData(Size) && Size > 0)
        {
            TArray<uint8> Data;
            Data.SetNumUninitialized(Size);
            int32 Received = 0;
            Sock->Recv(Data.GetData(), Data.Num(), Received);
            FString RespStr = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Data.GetData())));
            // Parse response JSON
            TSharedPtr<FJsonObject> RespObj;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RespStr);
            if (FJsonSerializer::Deserialize(Reader, RespObj) && RespObj.IsValid())
            {
                // Expect a field "prediction" with x,y,z
                if (RespObj->HasField("prediction"))
                {
                    TSharedPtr<FJsonObject> PredObj = RespObj->GetObjectField("prediction");
                    double x = PredObj->GetNumberField("x");
                    double y = PredObj->GetNumberField("y");
                    double z = PredObj->GetNumberField("z");
                    Result.NewLocation = FVector(x, y, z);
                }
            }
        }
    }
    return Result;
}
