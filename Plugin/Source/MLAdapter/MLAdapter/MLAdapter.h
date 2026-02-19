#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

class FMLAdapterModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void StartServer();
    void StopServer();
    void ListenerThread();

    FSocket* ListenerSocket = nullptr;
    FSocket* ConnectionSocket = nullptr;
    FRunnableThread* Thread = nullptr;
    FThreadSafeBool bShouldStop = false;
};

// Helper functions to serialize/deserialize JSON messages
FString ReceiveJson(FSocket* Socket);
bool SendJson(FSocket* Socket, const FString& JsonString);
