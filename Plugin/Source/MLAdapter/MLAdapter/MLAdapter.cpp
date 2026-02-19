#include "MLAdapter.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Json.h"
#include "Misc/ScopeLock.h"

#define LOG_CATEGORY LogTemp

// Global pointer to the module instance for static access
static FMLAdapterModule* GMLAdapterModule = nullptr;

void FMLAdapterModule::StartupModule()
{
    GMLAdapterModule = this;
    StartServer();
    UE_LOG(LOG_CATEGORY, Log, TEXT("MLAdapter module started, server listening"));
}

void FMLAdapterModule::ShutdownModule()
{
    bShouldStop = true;
    StopServer();
    UE_LOG(LOG_CATEGORY, Log, TEXT("MLAdapter module stopped"));
    GMLAdapterModule = nullptr;
}

void FMLAdapterModule::StartServer()
{
    // Create a TCP listen socket on port 5555
    ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
    ListenerSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("MLAdapterListener"), false);
    TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
    Addr->SetAnyAddress();
    Addr->SetPort(5555);
    bool bBound = ListenerSocket->Bind(*Addr);
    if (!bBound)
    {
        UE_LOG(LOG_CATEGORY, Error, TEXT("Failed to bind MLAdapter socket"));
        return;
    }
    ListenerSocket->Listen(1);
    Thread = FRunnableThread::Create([this](){ ListenerThread(); }, TEXT("MLAdapterListenerThread"));
}

void FMLAdapterModule::StopServer()
{
    if (ConnectionSocket)
    {
        ConnectionSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
        ConnectionSocket = nullptr;
    }
    if (ListenerSocket)
    {
        ListenerSocket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenerSocket);
        ListenerSocket = nullptr;
    }
    if (Thread)
    {
        Thread->Kill(true);
        delete Thread;
        Thread = nullptr;
    }
}

void FMLAdapterModule::ListenerThread()
{
    while (!bShouldStop)
    {
        bool bPending;
        if (ListenerSocket->HasPendingConnection(bPending) && bPending)
        {
            TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
            ConnectionSocket = ListenerSocket->Accept(*RemoteAddr, TEXT("MLAdapterClient"));
            if (ConnectionSocket)
            {
                UE_LOG(LOG_CATEGORY, Log, TEXT("MLAdapter client connected"));
                // Simple echo loop
                while (!bShouldStop && ConnectionSocket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
                {
                    FString Received = ReceiveJson(ConnectionSocket);
                    if (!Received.IsEmpty())
                    {
                        // Echo back (or modify as needed)
                        SendJson(ConnectionSocket, Received);
                    }
                }
                UE_LOG(LOG_CATEGORY, Log, TEXT("MLAdapter client disconnected"));
                ConnectionSocket->Close();
                ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
                ConnectionSocket = nullptr;
            }
        }
        FPlatformProcess::Sleep(0.1f);
    }
}

FString ReceiveJson(FSocket* Socket)
{
    uint32 Size;
    while (!Socket->HasPendingData(Size))
    {
        FPlatformProcess::Sleep(0.01f);
    }
    TArray<uint8> Data;
    Data.SetNumUninitialized(Size);
    int32 BytesRead = 0;
    Socket->Recv(Data.GetData(), Data.Num(), BytesRead);
    FString JsonString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(Data.GetData())));
    return JsonString;
}

bool SendJson(FSocket* Socket, const FString& JsonString)
{
    FTCHARToUTF8 Convert(*JsonString);
    int32 BytesSent = 0;
    return Socket->Send((uint8*)Convert.Get(), Convert.Length(), BytesSent);
}

IMPLEMENT_MODULE(FMLAdapterModule, MLAdapter)
